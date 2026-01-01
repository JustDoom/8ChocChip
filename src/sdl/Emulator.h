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
    
    const std::string &path;
    std::string sha1;
    bool encounteredError = false;
    bool isStopped = false;
    void loadState();
public:
    Emulator(const std::string& path, const RomSettings& romSettings, std::unordered_map<uint8_t, unsigned char> keymap);

    void init() override;
    bool handleEvent(SDL_Event* event) override;
    void update() override;
    void render() override;
    void resize(SDL_Event* event) override;
    
    void handleSaveState();
    void saveState(std::string path);
    int getInstructions();
    void resetInstructions();
};


#endif