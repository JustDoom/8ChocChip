#include "Emulator.h"
#include "../util/Constants.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#include "../util/MiscUtil.h"

Emulator::Emulator(const std::string& rom, const RomSettings& romSettings, std::unordered_map<uint8_t, unsigned char> keymap) : cpu(&renderWrapper, &keyboard, &speaker, romSettings, keymap), rom(rom) {
    this->keyboard.keymap = keymap;
}

void Emulator::init() {
    Window::init(64 * 15, 32 * 15);

    if (this->rom.empty()) {
        std::cerr << "No ROM file has been specified :(" << std::endl;
        return;
    }

    std::ifstream file(this->rom, std::ios::binary | std::ios::ate);
    if (!file.good()) {
        std::cerr << "Can not find file " << this->rom << std::endl;
        return;
    }
    SDL_SetWindowTitle(this->window, (std::string(SDL_GetWindowTitle(this->window)) + " - " + getFileFromStringPath(this->rom)).c_str());

    // Setup the emulator
    if (stringEndsWith(rom, ".state")) {
        this->loadState();
    } else {
        this->cpu.loadProgramIntoMemory(&file);
        this->cpu.loadSpritesIntoMemory();
    }
}

bool Emulator::handleEvent(SDL_Event& event) {
    if (!Window::handleEvent(event)) {
        return false;
    }

    switch (event.type) {
        case SDL_EVENT_KEY_DOWN:
            if (event.key.scancode == SDL_SCANCODE_F12) {
                this->handleSaveState();
            }
            this->keyboard.handleKeyDown(event.key.scancode);
            break;
        case SDL_EVENT_KEY_UP:
            this->keyboard.handleKeyUp(event.key.scancode);
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
        std::cout << "Program cancelled, Unknown opcode - " << opcode << std::endl;
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

void Emulator::resize(SDL_Event &event) {

}

void Emulator::handleSaveState() {
    SDL_DialogFileFilter filters[] = {
        {"8ChocChip State File", "state"}
    };

    std::string defaultLocation = this->rom;

    this->isStopped = true;
    SDL_ShowSaveFileDialog([](void* userData, const char* const* selectedPath, int filter) {
        const auto data = reinterpret_cast<Emulator*>(userData);
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

void Emulator::saveState(std::string path) {
    std::ofstream fileWriter;
    fileWriter.open(path, std::ios::binary);
    auto serialization = this->cpu.serialize();
    fileWriter.write((char*)serialization.data(), serialization.size());
    fileWriter.close();
}

void Emulator::loadState() {
    std::ifstream fileReader;
    fileReader.open(this->rom, std::ios::binary);
    
    if (!fileReader.is_open()) {
        std::cerr << "Error opening file '" << this->rom << "'" << std::endl;
        return;
    }

    std::vector<uint8_t> buffer(this->cpu.serializationDimension);

    fileReader.seekg(0, std::ios::end);
    size_t file_size = fileReader.tellg();
    fileReader.seekg(0, std::ios::beg);
    
    fileReader.read(reinterpret_cast<char*>(&buffer[0]), file_size);
    fileReader.close();

    this->cpu.deserialize(buffer.data());
}

int Emulator::getInstructions() {
    return this->cpu.instructions;
}

void Emulator::resetInstructions() {
    this->cpu.instructions = 0;
}

