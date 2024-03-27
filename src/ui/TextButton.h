//
// Created by doom on 3/27/24.
//

#ifndef TEXTBUTTON_H
#define TEXTBUTTON_H

#include "SFML/Graphics.hpp"


class TextButton {
private:
    sf::RectangleShape button;
    sf::Text text;
    sf::Font font;
    sf::Color idleColor;
    sf::Color hoverColor;
    sf::Color activeColor;
    bool isPressed;
    bool isHovered;

public:
    TextButton(float x, float y, float width, float height, std::string buttonText);

    void updateSize(float width, float height);

    void update(sf::RenderWindow& window);

    void draw(sf::RenderWindow& window);

    bool isClicked();
};



#endif //TEXTBUTTON_H
