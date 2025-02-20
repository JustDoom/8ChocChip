#ifndef MAINMENU_H
#define MAINMENU_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL3_ttf/SDL_ttf.h>

#include "InputHandler.h"
#include "Window.h"
#include "ui/MainBox.h"
#include "ui/ScrollBox.h"
#include "ui/TextButton.h"

class MainMenu : public Window {
private:
    std::string configFilePath;
    std::vector<std::string>& romDirectories;
    std::unordered_map<std::string*, std::vector<std::string>>& romFiles;
    std::vector<std::unique_ptr<Window>>& windows;

    std::unordered_map<std::string, TextButton> roms;
    std::shared_ptr<TextButton> chooseFolder;
    std::shared_ptr<ScrollBox> scrollRoms;
    std::shared_ptr<MainBox> mainBox;

    InputHandler inputHandler{};

    TTF_TextEngine* textEngine;
    TTF_Font* font = nullptr;
    SDL_Mutex* mutex;
public:
    MainMenu(TTF_Font* font, std::string configFilePath, std::unordered_map<std::string *,
        std::vector<std::string>>& romFiles, std::vector<std::string>& romDirectories,
        std::vector<std::unique_ptr<Window>>& windows);

    void init() override;
    bool handleEvent(SDL_Event& event) override;
    void update() override;
    void render() override;
    void resize(SDL_Event& event) override;
    void close() override;

    void launchRom(const std::string& file) const;
    std::vector<std::shared_ptr<Element>> refreshRoms();
    static void SDLCALL callback(void* userdata, const char* const* filelist, int filter);
};

#endif