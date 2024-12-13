#ifndef INC_8CHOCCHIP_INPUTHANDLER_H
#define INC_8CHOCCHIP_INPUTHANDLER_H

#include <SDL3/SDL_keycode.h>
#include <vector>

class InputHandler {
private:
    std::vector<SDL_Keycode> keys;
    std::vector<SDL_Keycode> mouse;

    std::vector<SDL_Keycode> lastKeys;
    std::vector<SDL_Keycode> lastMouse;
public:
    InputHandler() = default;

    void addKey(SDL_Keycode key);
    void removeKey(SDL_Keycode key);
    void updateLastKeys();
    bool isJustPressed(SDL_Keycode key);
    bool isPressed(SDL_Keycode key);

    void addButton(SDL_Keycode button);
    void removeButton(SDL_Keycode button);
    void updateLastMouse();
    bool isJustClicked(SDL_Keycode button);
    bool isClicked(SDL_Keycode button);
};


#endif