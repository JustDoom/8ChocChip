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

// void Emulator::focus() {
//     if (!mShown) {
//         SDL_ShowWindow(mWindow);
//     }
//
//     SDL_RaiseWindow(mWindow);
// }

void Emulator::update() {
    // Run a cycle of the emulator
    this->cpu.cycle();
}

void Emulator::render() {
    // Set renderer draw color
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 0);
    // Clear the window
    SDL_RenderClear(this->renderer);

    // Render the window
    this->renderWrapper.render(this->renderer);
    SDL_RenderPresent(this->renderer);

    SDL_Delay(16);
}
