#ifndef EMULATORENTRY_H
#define EMULATORENTRY_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Timer.h"
#include "SDL3/SDL_events.h"
#include "sdl/Window.h"

class EmulatorMain {
private:
    std::string rom;
    std::vector<std::unique_ptr<Window>> windows;
    std::vector<std::string> romDirectories;
    std::unordered_map<std::string*, std::vector<std::string>> romFiles;
    bool running = true;
    TTF_Font* font = nullptr;

    Timer fpsTimer;
    Timer capTimer;
    Timer fpsPrintTimer;

    int countedFrames = 0;
public:
    bool initialise(int argc, char** argv);
    void run();
    void event(SDL_Event* event);
    void quit();

    bool isRunning() const;
};

#endif