#include "Keyboard.h"

#include <utility>

void Keyboard::setOnNextKeyPress(std::function<void(unsigned char)> callback) {
    this->onNextKeyPress = std::move(callback);
}

bool Keyboard::isKeyPressed(int keyCode) {
    return keysPressed[keyCode];
}

void Keyboard::handleKeyDown(uint8_t keyCode) {
    auto keyMapIter = this->KEYMAP.find(keyCode);
    if (keyMapIter != this->KEYMAP.end()) {
        uint8_t key = keyMapIter->second;
        this->keysPressed[key] = true;
        if (this->onNextKeyPress) {
            this->onNextKeyPress(key);
            this->onNextKeyPress = nullptr;
        }
    }
}

void Keyboard::handleKeyUp(uint8_t keyCode) {
    auto keyMapIter = this->KEYMAP.find(keyCode);
    if (keyMapIter != this->KEYMAP.end()) {
        uint8_t key = keyMapIter->second;
        this->keysPressed[key] = false;
    }
}
