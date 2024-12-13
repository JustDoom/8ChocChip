#ifndef MAINMENU_H
#define MAINMENU_H

#include <unordered_map>
#include <vector>

#include <nfd.hpp>

#include "Emulator.h"
#include "InputHandler.h"
#include "Window.h"
#include "ui/TextButton.h"

class MainMenu : public Window {
private:
    std::string configFilePath;
    std::vector<std::string>& romDirectories;
    std::unordered_map<std::string*, std::vector<std::string>>& romFiles;
    std::vector<std::unique_ptr<Window>>& windows;

    NFD::Guard nfdGuard;
    NFD::UniquePath outPath;

    std::unordered_map<std::string, TextButton> roms;
    TextButton chooseFolder;

    InputHandler inputHandler{};
public:
    MainMenu(std::string configFilePath, std::unordered_map<std::string *, std::vector<std::string>>& romFiles,
             std::vector<std::string>& romDirectories, std::vector<std::unique_ptr<Window>>& windows);

    void init() override;
    bool handleEvent(SDL_Event& event) override;
    void update() override;
    void render() override;
};

#endif