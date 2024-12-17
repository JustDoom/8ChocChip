#include "Emulator.h"

Emulator::Emulator(const std::string &rom) : cpu(&renderWrapper, &keyboard, &speaker), rom(rom) {}

void Emulator::init() {
    Window::init();

    if (this->rom.empty()) {
        std::cerr << "No ROM file has been specified :(" << std::endl;
        return;
    }

    std::ifstream file(this->rom, std::ios::binary | std::ios::ate);
    if (!file.good()) {
        std::cerr << "Can not find file " << this->rom << std::endl;
        return;
    }

    cpu.loadSpritesIntoMemory();

    cpu.loadProgramIntoMemory(&file);
}

bool Emulator::handleEvent(SDL_Event& event) {
    if (!Window::handleEvent(event)) {
        return false;
    }

    switch (event.type) {
        case SDL_EVENT_KEY_DOWN:
            keyboard.handleKeyDown(event.key.scancode);
            break;
        case SDL_EVENT_KEY_UP:
            keyboard.handleKeyUp(event.key.scancode);
            break;
    }

    return true;
}

void Emulator::update() {
    // Run a cycle of the emulator
    this->cpu.cycle();
}

void Emulator::render() {
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 0);
    SDL_RenderClear(this->renderer);

    this->renderWrapper.render(this->renderer);
    SDL_RenderPresent(this->renderer);
}

void Emulator::resize(SDL_Event &event) {

}
