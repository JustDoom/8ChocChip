#include "Emulator.h"
#include "../util/Constants.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#include "../util/MiscUtil.h"

Emulator::Emulator(const std::string& path, const RomSettings& romSettings, const std::unordered_map<uint8_t, unsigned char>& keymap) : cpu(&renderWrapper, &keyboard, &speaker, romSettings, keymap), path(path), sha1(sha1FromFile(path)) {
    if (this->path.ends_with(".state")) {
        std::ifstream fileReader;
        fileReader.open(path, std::ios::binary);
        
        if (!fileReader.is_open()) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error opening file '%s'", path.c_str());
            return;
        }

        std::vector<uint8_t> buffer(sha1Dimension);
    
        fileReader.seekg(0, std::ios::end);
        size_t file_size = fileReader.tellg();
        fileReader.seekg(Cpu::serializationDimension - 1, std::ios::beg);
        
        fileReader.read(reinterpret_cast<char*>(&buffer[0]), sha1Dimension);
        fileReader.close();

        std::string stateSha1(buffer.begin(), buffer.end());
        this->sha1 = stateSha1;
    } else {
        this->sha1 = sha1FromFile(this->path);
    }
    this->keyboard.keymap = keymap;
}

#ifdef __EMSCRIPTEN__
void Emulator::init(SDL_Window* window, SDL_Renderer* renderer) {
    Window::init(window, renderer);
#else
void Emulator::init() {
    Window::init(64 * 15, 32 * 15);
#endif

    if (this->path.empty()) {
        std::cerr << "No ROM file has been specified :(" << std::endl;
        return;
    }

    std::ifstream file(this->path, std::ios::binary | std::ios::ate);
    if (!file.good()) {
        std::cerr << "Can not find file " << this->path << std::endl;
        return;
    }
    SDL_SetWindowTitle(this->window, (std::string(SDL_GetWindowTitle(this->window)) + " - " + getFileFromStringPath(this->path)).c_str());

    // Setup the emulator
    if (stringEndsWith(this->path, ".state")) {
        this->loadState();
    } else {
        this->cpu.loadProgramIntoMemory(&file);
        this->cpu.loadSpritesIntoMemory();
    }

    SDL_Log("Loaded");
}

bool Emulator::handleEvent(SDL_Event* event) {
    if (!Window::handleEvent(event)) {
        return false;
    }

    switch (event->type) {
        case SDL_EVENT_KEY_DOWN:
            if (event->key.scancode == SDL_SCANCODE_F12) {
                this->handleSaveState();
            }
            this->keyboard.handleKeyDown(event->key.scancode);
            break;
        case SDL_EVENT_KEY_UP:
            this->keyboard.handleKeyUp(event->key.scancode);
            break;
    }

    return true;
}

void Emulator::update() {
    try {
        if (!this->encounteredError && !this->isStopped) {
            this->cpu.cycle();
        }
    } catch (uint16_t opcode) {
        SDL_Log("Program cancelled, Unknown opcode - %s", opcode);
        this->encounteredError = true;
        SDL_SetWindowTitle(this->window, ("Program cancelled, Unknown opcode - " + std::to_string(opcode)).c_str());
    }
}

void Emulator::render() {
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 0);
    SDL_RenderClear(this->renderer);

    this->renderWrapper.render(this->renderer);
    SDL_RenderPresent(this->renderer);
}

void Emulator::resize(SDL_Event* event) {

}

void Emulator::handleSaveState() {
    constexpr SDL_DialogFileFilter filters[] = {
        {"8ChocChip State File", "state"}
    };

    const std::string defaultLocation = this->path;

    this->isStopped = true;
    SDL_ShowSaveFileDialog([](void* userData, const char* const* selectedPath, int filter) {
        const auto data = static_cast<Emulator*>(userData);
        data->isStopped = false;
        
        if (!selectedPath || !*selectedPath) {
            SDL_Log("The user did not select any file. Most likely, the dialog was canceled.");
            return;
        }

        std::filesystem::path path(*selectedPath);
        if (path.extension() != ".state") {
            path += ".state";
        }

        data->saveState(path.string());
    }, this, this->window, filters, 1, defaultLocation.c_str());
}

void Emulator::saveState(const std::string& path) {
    std::ofstream fileWriter;
    fileWriter.open(path, std::ios::binary);
    auto serialization = this->cpu.serialize();
    
    // After the CPU serialization, I insert the SHA1 of the ROM to check it when selecting a state
    // in the main dialog
    serialization.insert(serialization.end(), this->sha1.cbegin(), this->sha1.cend());
    
    fileWriter.write(reinterpret_cast<char*>(serialization.data()), serialization.size());
    fileWriter.close();
}

void Emulator::loadState() {
    std::ifstream fileReader;
    fileReader.open(this->path, std::ios::binary);
    
    if (!fileReader.is_open()) {
        std::cerr << "Error opening file '" << this->path << "'" << std::endl;
        return;
    }

    std::vector<uint8_t> buffer(Cpu::serializationDimension + sha1Dimension);

    fileReader.seekg(0, std::ios::end);
    const size_t file_size = fileReader.tellg();
    fileReader.seekg(0, std::ios::beg);
    
    fileReader.read(reinterpret_cast<char*>(&buffer[0]), file_size);
    fileReader.close();

    this->cpu.deserialize(buffer.data());
}

int Emulator::getInstructions() const {
    return this->cpu.instructions;
}

void Emulator::resetInstructions() {
    this->cpu.instructions = 0;
}

