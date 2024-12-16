#include "InputHandler.h"

#include <algorithm>
#include <iostream>

void InputHandler::addKey(SDL_Keycode key) {
    removeKey(key); // Do this because when a button is held down it adds many into the list and breaks functionality\
    // If the key is already in the list this will remove it, if not it wont do anything
    this->keys.emplace_back(key);
}

void InputHandler::removeKey(SDL_Keycode key) {
    auto it = std::find(this->keys.begin(), this->keys.end(), key);
    if (it != this->keys.end()) {
        this->keys.erase(it);
    }
}

void InputHandler::updateLastKeys() {
    this->lastKeys.clear();
    for (const auto &key: this->keys) {
        this->lastKeys.emplace_back(key);
    }
}

bool InputHandler::isJustPressed(SDL_Keycode key) {
    return std::count(this->keys.begin(), this->keys.end(), key) > 0 && std::count(this->lastKeys.begin(), this->lastKeys.end(), key) == 0;
}

bool InputHandler::isPressed(SDL_Keycode key) {
    return std::count(this->keys.begin(), this->keys.end(), key);
}

void InputHandler::addButton(SDL_Keycode button) {
    removeButton(button);
    this->mouse.emplace_back(button);
}

void InputHandler::removeButton(SDL_Keycode button) {
    auto it = std::find(this->mouse.begin(), this->mouse.end(), button);
    if (it != this->mouse.end()) {
        this->mouse.erase(it);
    }
}

void InputHandler::updateLastMouse() {
    this->lastMouse.clear();
    for (const auto &key: this->mouse) {
        this->lastMouse.emplace_back(key);
    }
}

bool InputHandler::isJustClicked(SDL_Keycode button) {
    return std::count(this->mouse.begin(), this->mouse.end(), button) && !std::count(this->lastMouse.begin(), this->lastMouse.end(), button);
}

bool InputHandler::isClicked(SDL_Keycode button) {
    return std::count(this->mouse.begin(), this->mouse.end(), button);
}