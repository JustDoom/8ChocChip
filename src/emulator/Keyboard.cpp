#include "Keyboard.h"

#include <utility>

void Keyboard::setOnNextKeyPress(std::function<void(unsigned char)> callback) {
    this->onNextKeyPress = std::move(callback);
}

bool Keyboard::isKeyPressed(int keyCode) {
    return keysPressed[keyCode];
}
