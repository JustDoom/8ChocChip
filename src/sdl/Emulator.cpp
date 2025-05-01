#include "Emulator.h"

#include <iostream>

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

    this->cpu.loadSpritesIntoMemory();
    this->cpu.loadProgramIntoMemory(&file);
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

int Emulator::getInstructions() {
    return this->cpu.instructions;
}

void Emulator::resetInstructions() {
    this->cpu.instructions = 0;
}

