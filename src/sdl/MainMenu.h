#ifndef MAINMENU_H
#define MAINMENU_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL3_ttf/SDL_ttf.h>

#include "InputHandler.h"
#include "Window.h"
#include "../Settings.h"
#include "Database.h"
#include "../util/Constants.h"

#include "../../dependencies/clay/clay.h"

class MainMenu;

struct HoverData {
    MainMenu* self;
    std::string* data;
};

class MainMenu final : public Window {
private:
    std::vector<std::string>& romDirectories;
    std::unordered_map<std::string*, std::vector<std::string>>& romFiles;
    std::vector<std::unique_ptr<Window>>& windows;

    InputHandler inputHandler{};
    SDL_Mutex* mutex;
    std::vector<HoverData> dataList;

    std::string* selectedRom = nullptr;
    std::string* selectedState = nullptr;
    std::string* selectedPlatformId = nullptr;
    RomSettings romSettings;

    Database database;
    bool isKeymapMenuOpen = false;
    
    std::unordered_map<uint8_t, unsigned char> getSelectedRomKeymap() const;
public:
    MainMenu(TTF_Font* font, std::unordered_map<std::string *,
        std::vector<std::string>>& romFiles, 
        std::vector<std::string>& romDirectories,
        std::vector<std::unique_ptr<Window>>& windows);

    void init() override;
    bool handleEvent(SDL_Event* event) override;
    void update() override;
    void render() override;
    void resize(SDL_Event* event) override;
    void close() override;

    static void handleRomClick(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData);
    static void handlePlatformClick(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData);
    static void handleAddNewRom(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData);
    static void handlePlay(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData);
    static void handleRefresh(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData);
    static void SDLCALL callback(void* userdata, const char* const* filelist, int filter);
    static void SDLCALL selectStateCallback(void* userdata, const char* const* filelist, int filter);
};

#endif