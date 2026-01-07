#include "InputHandler.h"

#include <algorithm>

void InputHandler::addKey(SDL_Keycode key) {
    removeKey(key); // Do this because when a button is held down it adds many into the list and breaks functionality\
    // If the key is already in the list this will remove it, if not it won't do anything
    this->keys.emplace_back(key);
}

void InputHandler::removeKey(const SDL_Keycode key) {
    if (const auto it = std::ranges::find(this->keys, key); it != this->keys.end()) {
        this->keys.erase(it);
    }
}

void InputHandler::updateLastKeys() {
    this->lastKeys.clear();
    for (const auto &key: this->keys) {
        this->lastKeys.emplace_back(key);
    }
}

bool InputHandler::isJustPressed(const SDL_Keycode key) {
    return std::ranges::count(this->keys, key) > 0 && std::ranges::count(this->lastKeys, key) == 0;
}

bool InputHandler::isPressed(const SDL_Keycode key) {
    return std::ranges::count(this->keys, key);
}

void InputHandler::addButton(SDL_Keycode button) {
    removeButton(button);
    this->mouse.emplace_back(button);
}

void InputHandler::removeButton(const SDL_Keycode button) {
    if (const auto it = std::ranges::find(this->mouse, button); it != this->mouse.end()) {
        this->mouse.erase(it);
    }
}

void InputHandler::updateLastMouse() {
    this->lastMouse.clear();
    for (const auto &key: this->mouse) {
        this->lastMouse.emplace_back(key);
    }
}

bool InputHandler::isJustClicked(const SDL_Keycode button) {
    return std::ranges::count(this->mouse, button) && !std::ranges::count(this->lastMouse, button);
}

bool InputHandler::isClicked(const SDL_Keycode button) {
    return std::ranges::count(this->mouse, button);
}