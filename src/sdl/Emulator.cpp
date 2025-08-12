#include "Emulator.h"
#include "../util/Constants.h"

#include <iostream>
#include <fstream>

#include "../util/MiscUtil.h"

Emulator::Emulator(const std::string& rom, const RomSettings& romSettings) : cpu(&renderWrapper, &keyboard, &speaker, romSettings), rom(rom) {}

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
    this->cpu.loadSpritesIntoMemory();
    this->cpu.loadProgramIntoMemory(&file);

    this->loadState();
}

bool Emulator::handleEvent(SDL_Event& event) {
    if (!Window::handleEvent(event)) {
        return false;
    }

    switch (event.type) {
        case SDL_EVENT_KEY_DOWN:
            this->keyboard.handleKeyDown(event.key.scancode);
            break;
        case SDL_EVENT_KEY_UP:
            this->keyboard.handleKeyUp(event.key.scancode);
            break;
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            this->saveState();
            break;
    }

    return true;
}

void Emulator::update() {
    try {
        if (!this->encounteredError) {
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

void Emulator::saveState() {
    if (!home) {
        std::cerr << home << " environment variable not set. " << std::endl;
        return;
    }
    
    std::filesystem::path romFilePath(this->rom);
    std::string stateFileName = romFilePath.filename().string() + ".state";
    std::string stateFilePath = (std::filesystem::path(home) / stateFileName).string();
    
    std::ofstream fileWriter;
    fileWriter.open(stateFilePath, std::ios::binary);
    auto serialization = this->cpu.serialize();
    fileWriter.write((char*)serialization.data(), serialization.size());
    fileWriter.close();
}

void Emulator::loadState() {
    if (!home) {
        std::cerr << home << " environment variable not set. " << std::endl;
        return;
    }
    
    std::filesystem::path romFilePath(this->rom);
    std::string stateFileName = romFilePath.filename().string() + ".state";
    std::string stateFilePath = (std::filesystem::path(home) / stateFileName).string();

    std::ifstream fileReader;
    fileReader.open(stateFilePath, std::ios::binary);
    
    if (!fileReader.is_open()) {
        std::cerr << "Status file not found for rom '" << this->rom << "'" << std::endl;
        return;
    }

    std::vector<uint8_t> buffer(this->cpu.serializationDimension);

    fileReader.seekg(0, std::ios::end);
    size_t file_size = fileReader.tellg();
    fileReader.seekg(0, std::ios::beg);
    
    fileReader.read(reinterpret_cast<char*>(&buffer[0]), file_size);
    fileReader.close();

    this->cpu.deserialize(buffer);
}

int Emulator::getInstructions() {
    return this->cpu.instructions;
}

void Emulator::resetInstructions() {
    this->cpu.instructions = 0;
}

