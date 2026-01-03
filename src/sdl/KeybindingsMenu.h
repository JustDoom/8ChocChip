#ifndef KEYBINDINGS_MENU_H
#define KEYBINDINGS_MENU_H

#include <vector>
#include <string>
#include <unordered_map>

#include <SDL3_ttf/SDL_ttf.h>

#include "Window.h"

#include "../../dependencies/clay/clay.h"

class KeybindingsMenu;

struct KeybindingHoverData {
    KeybindingsMenu* self;
    uint8_t* key;
};

class KeybindingsMenu final : public Window {
private:
    std::vector<KeybindingHoverData> dataList;
    uint8_t* keyWaitingFor = nullptr;
    std::unordered_map<uint8_t, unsigned char> keymap;
    bool* isMenuOpen;
    std::string romSha1;

    SDL_Mutex* mutex;

    char getKeyboardCharacter(int key_code) const;
public:
    KeybindingsMenu(TTF_Font* font, std::string romSha1, bool* isMenuOpen);

    void init() override;
    bool handleEvent(SDL_Event* event) override;
    void update() override;
    void render() override;
    void close() override;

    static void handleKeybindingClick(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData);
    static void handleResetKeybindings(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData);
};

#endif