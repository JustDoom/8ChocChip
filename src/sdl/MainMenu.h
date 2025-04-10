#ifndef MAINMENU_H
#define MAINMENU_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL3_ttf/SDL_ttf.h>

#include "InputHandler.h"
#include "Window.h"

#include "../../dependencies/clay/clay.h"
#include "../ClaySDL3Renderer.h"

class MainMenu : public Window {
private:
    std::vector<std::string>& romDirectories;
    std::unordered_map<std::string*, std::vector<std::string>>& romFiles;
    std::vector<std::unique_ptr<Window>>& windows;

    InputHandler inputHandler{};
    SDL_Mutex* mutex;
public:
    MainMenu(TTF_Font* font, std::unordered_map<std::string *,
        std::vector<std::string>>& romFiles, std::vector<std::string>& romDirectories,
        std::vector<std::unique_ptr<Window>>& windows);

    void init() override;
    bool handleEvent(SDL_Event& event) override;
    void update() override;
    void render() override;
    void resize(SDL_Event& event) override;
    void close() override;

    static void SDLCALL callback(void* userdata, const char* const* filelist, int filter);
};

#endif