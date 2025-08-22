#ifndef KEYBINDINGS_MENU_H
#define KEYBINDINGS_MENU_H

#include <vector>
#include <string>

#include <SDL3_ttf/SDL_ttf.h>

#include "Window.h"

#include "../../dependencies/clay/clay.h"

class KeybindingsMenu;

struct KeybindingHoverData {
    KeybindingsMenu* self;
    std::string* key;
};

class KeybindingsMenu final : public Window {
private:
    std::vector<KeybindingHoverData> dataList;
    std::string* keyWaitingFor = nullptr;

public:
    KeybindingsMenu(TTF_Font* font);

    void init() override;
    bool handleEvent(SDL_Event& event) override;
    void update() override;
    void render() override;
    void resize(SDL_Event& event) override;
    void close() override;

    static void handleKeybindClick(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData);
};

#endif