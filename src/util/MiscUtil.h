#ifndef MISCUTIL_H
#define MISCUTIL_H

#include <string>
#include <vector>
#include <algorithm>

#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Mouse.hpp"

class MiscUtil {
public:
    static std::vector<sf::Keyboard::Key> keys;
    static std::vector<sf::Mouse::Button> mouse;

    static std::string toLowerCase(const std::string& string);

    static std::string getFileFromPath(std::string path);

    static void addKey(sf::Keyboard::Key key);
    static void removeKey(sf::Keyboard::Key key);
    static bool isPressed(sf::Keyboard::Key key);

    static void addButton(sf::Mouse::Button button);
    static void removeButon(sf::Mouse::Button button);
    static bool isClicked(sf::Mouse::Button button);
};

#endif //MISCUTIL_H
