#ifndef INC_8CHOCCHIP_SDL_H
#define INC_8CHOCCHIP_SDL_H

#include "../emulator/Cpu.h"
#include "Window.h"
#include "../emulator/Keyboard.h"
#include "../emulator/Speaker.h"

#include "../Settings.h"

class Emulator : public Window {
private:
    Speaker speaker;
    Keyboard keyboard;
    Cpu cpu;

    float scale = 15;
    int offsetX = 0;
    int offsetY = 0;
    
    const std::string &path;
    std::string sha1;
    bool encounteredError = false;
    bool isStopped = false;
    void loadState();
public:
    Emulator(const std::string& path, const RomSettings& romSettings, const std::unordered_map<uint8_t, unsigned char>& keymap);

#ifdef __EMSCRIPTEN__
    void init(SDL_Window* window, SDL_Renderer* renderer);
#else
    void init() override;
#endif
    bool handleEvent(SDL_Event* event) override;
    void update() override;
    void render() override;
    void resize(SDL_Event* event) override;
    
    void handleSaveState();
    void saveState(const std::string& path);
    int getInstructions() const;
    void resetInstructions();
};


#endif