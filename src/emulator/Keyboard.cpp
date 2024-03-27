//
// Created by doom on 19/03/24.
//

#include "Keyboard.h"

#include <utility>

void Keyboard::setOnNextKeyPress(std::function<void(unsigned char)> callback) {
    this->onNextKeyPress = std::move(callback);
}

void Keyboard::handleEvent(sf::Event event) {
    if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
        int keyCode = event.key.code;
        auto keyMapIter = this->KEYMAP.find(keyCode);
        if (keyMapIter != this->KEYMAP.end()) {
            uint8_t key = keyMapIter->second;
            if (event.type == sf::Event::KeyPressed) {
                this->keysPressed[key] = true;
                if (this->onNextKeyPress) {
                    this->onNextKeyPress(key);
                    this->onNextKeyPress = nullptr;
                }
            } else {
                this->keysPressed[key] = false;
            }
        }
    }
}


bool Keyboard::isKeyPressed(int keyCode) {
    return keysPressed[keyCode];
}
