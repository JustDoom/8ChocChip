#include "SdlKeyboard.h"

void SdlKeyboard::handleKeyDown(uint8_t keyCode) {
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

void SdlKeyboard::handleKeyUp(uint8_t keyCode) {
    auto keyMapIter = this->KEYMAP.find(keyCode);
    if (keyMapIter != this->KEYMAP.end()) {
        uint8_t key = keyMapIter->second;
        this->keysPressed[key] = false;
    }
}
