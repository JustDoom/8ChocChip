#include "InputHandler.h"

void InputHandler::addKey(sf::Keyboard::Key key) {
    this->keys.emplace_back(key);
}

void InputHandler::removeKey(sf::Keyboard::Key key) {
    auto it = std::find(this->keys.begin(), this->keys.end(), key);
    if (it != this->keys.end()) {
        this->keys.erase(it);
    }
}

bool InputHandler::isPressed(sf::Keyboard::Key key) {
    return std::count(this->keys.begin(), this->keys.end(), key);
}

void InputHandler::addButton(sf::Mouse::Button button) {
    this->mouse.emplace_back(button);
}

void InputHandler::removeButton(sf::Mouse::Button button) {
    auto it = std::find(this->mouse.begin(), this->mouse.end(), button);
    if (it != this->mouse.end()) {
        this->mouse.erase(it);
    }
}

bool InputHandler::isClicked(sf::Mouse::Button button) {
    return std::count(this->mouse.begin(), this->mouse.end(), button);
}