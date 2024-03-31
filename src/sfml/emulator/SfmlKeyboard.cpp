#include "SfmlKeyboard.h"

void SfmlKeyboard::handleKeyDown(uint8_t keyCode) {
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

void SfmlKeyboard::handleKeyUp(uint8_t keyCode) {
    auto keyMapIter = this->KEYMAP.find(keyCode);
    if (keyMapIter != this->KEYMAP.end()) {
        uint8_t key = keyMapIter->second;
        this->keysPressed[key] = false;
    }
}
