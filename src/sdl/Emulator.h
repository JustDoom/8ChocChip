#ifndef INC_8CHOCCHIP_SDL_H
#define INC_8CHOCCHIP_SDL_H

#include "Window.h"
#include "../emulator/Cpu.h"
#include "emulator/SdlKeyboard.h"
#include "emulator/SdlRenderer.h"
#include "emulator/SdlSpeaker.h"

class Emulator : public Window {
private:
    SdlRenderer renderWrapper;
    SdlSpeaker speaker;
    SdlKeyboard keyboard;
    Cpu cpu;

    const std::string& rom;
public:
    Emulator(const std::string& rom);

    void init() override;
    bool handleEvent(SDL_Event& event) override;
    void update() override;
    void render() override;
    void resize(SDL_Event &event) override;
};


#endif