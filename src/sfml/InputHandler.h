#ifndef INC_8CHOCCHIP_INPUTHANDLER_H
#define INC_8CHOCCHIP_INPUTHANDLER_H

#include <algorithm>
#include <vector>

#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Mouse.hpp"

class InputHandler {
private:
    std::vector<sf::Keyboard::Key> keys;
    std::vector<sf::Mouse::Button> mouse;

    std::vector<sf::Keyboard::Key> lastKeys;
    std::vector<sf::Mouse::Button> lastMouse;
public:
    InputHandler() = default;

    void addKey(sf::Keyboard::Key key);
    void removeKey(sf::Keyboard::Key key);
    void updateLastKeys();
    bool isJustPressed(sf::Keyboard::Key key);
    bool isPressed(sf::Keyboard::Key key);

    void addButton(sf::Mouse::Button button);
    void removeButton(sf::Mouse::Button button);
    void updateLastMouse();
    bool isJustClicked(sf::Mouse::Button button);
    bool isClicked(sf::Mouse::Button button);
};


#endif // INC_8CHOCCHIP_INPUTHANDLER_H
