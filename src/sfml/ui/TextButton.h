//
// Created by doom on 3/27/24.
//

#ifndef TEXTBUTTON_H
#define TEXTBUTTON_H

#include "SFML/Graphics.hpp"


class TextButton {
private:
    sf::RectangleShape button;
    sf::Vector2f originalSize;
    sf::Vector2f originalPosition;
    sf::Text text;
    sf::Font* font;
    sf::Color idleColor;
    sf::Color hoverColor;
    sf::Color activeColor;
    bool isPressed;
    bool lastPressed;
    bool isHovered;

public:
    TextButton(float x, float y, float width, float height, std::string buttonText, sf::Font* font);

    void updateSize(const sf::Vector2u originalSize, const sf::Vector2u updatedSize);

    void update(sf::RenderWindow& window, sf::Vector2i pos);

    void draw(sf::RenderWindow& window);

    bool isClicked();

    bool isJustClicked();
};



#endif //TEXTBUTTON_H
