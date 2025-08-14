#ifndef INC_8CHOCCHIP_SDL_H
#define INC_8CHOCCHIP_SDL_H

#include "../emulator/Cpu.h"
#include "Window.h"
#include "../emulator/Keyboard.h"
#include "../emulator/Renderer.h"
#include "../emulator/Speaker.h"

#include "../Settings.h"

class Emulator : public Window {
private:
    Renderer renderWrapper;
    Speaker speaker;
    Keyboard keyboard;
    Cpu cpu;

    const std::string &rom;
    bool encounteredError = false;
public:
    Emulator(const std::string& rom, const RomSettings& romSettings, std::string statePath);

    void init() override;
    bool handleEvent(SDL_Event& event) override;
    void update() override;
    void render() override;
    void resize(SDL_Event &event) override;
    
    void handleSaveState();
    void saveState(std::string path);
    void loadState(std::string path);
    int getInstructions();
    void resetInstructions();
};


#endif