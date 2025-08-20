#ifndef KeybindingsMenu_H
#define KeybindingsMenu_H

#include "Window.h"

class KeybindingsMenu;

class KeybindingsMenu final : public Window {
private:
public:
    KeybindingsMenu(TTF_Font* font);

    void init() override;
    bool handleEvent(SDL_Event& event) override;
    void update() override;
    void render() override;
    void resize(SDL_Event& event) override;
    void close() override;
};

#endif