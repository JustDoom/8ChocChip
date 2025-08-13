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
    const std::string &state = "";
    bool encounteredError = false;
public:
    Emulator(const std::string& rom, const RomSettings& romSettings, const std::string& state);

    void init() override;
    bool handleEvent(SDL_Event& event) override;
    void update() override;
    void render() override;
    void resize(SDL_Event &event) override;
    
    void saveState();
    void loadState();
    int getInstructions();
    void resetInstructions();
};


#endif