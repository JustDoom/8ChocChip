#include "SdlKeyboard.h"

void SdlKeyboard::handleKeyDown(const uint8_t keyCode) {
    if (const auto keyMapIter = this->KEYMAP.find(keyCode); keyMapIter != this->KEYMAP.end()) {
        const uint8_t key = keyMapIter->second;
        this->keysPressed[key] = true;
        if (this->onNextKeyPress) {
            this->onNextKeyPress(key);
            this->onNextKeyPress = nullptr;
        }
    }
}

void SdlKeyboard::handleKeyUp(uint8_t keyCode) {
    if (const auto keyMapIter = this->KEYMAP.find(keyCode); keyMapIter != this->KEYMAP.end()) {
        const uint8_t key = keyMapIter->second;
        this->keysPressed[key] = false;
    }
}
