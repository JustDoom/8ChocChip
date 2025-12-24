#include <iostream>

#include "Keyboard.h"

void Keyboard::setOnNextKeyPress(std::function<void(unsigned char)> callback) {
    this->onNextKeyPress = std::move(callback);
}

bool Keyboard::isKeyPressed(const int keyCode) {
    return keysPressed[keyCode];
}

void Keyboard::handleKeyDown(const uint8_t keyCode) {
    if (const auto keyMapIter = this->keymap.find(keyCode); keyMapIter != this->keymap.end()) {
        const uint8_t key = keyMapIter->second;
        this->keysPressed[key] = true;
        if (this->onNextKeyPress) {
            this->onNextKeyPress(key);
            this->onNextKeyPress = nullptr;
        }
    }
}

void Keyboard::handleKeyUp(const uint8_t keyCode) {
    if (const auto keyMapIter = this->keymap.find(keyCode); keyMapIter != this->keymap.end()) {
        const uint8_t key = keyMapIter->second;
        this->keysPressed[key] = false;
    }
}
