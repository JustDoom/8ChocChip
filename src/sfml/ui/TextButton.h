#ifndef TEXTBUTTON_H
#define TEXTBUTTON_H

#include "SFML/Graphics.hpp"

#include "../InputHandler.h"

class TextButton {
private:
    sf::RectangleShape button;
    sf::Vector2f originalSize;
    sf::Vector2f originalPosition;
    sf::Text text;
    sf::Font* font{};
    sf::Color color;
    sf::Color idleColor;
    sf::Color hoverColor;
    sf::Color activeColor;
    bool isPressed;
    bool lastPressed{};
    bool isHovered;

public:
    TextButton(float x, float y, float width, float height, const std::string& buttonText, sf::Font *font);

    void updateSize(sf::Vector2u originalSize, sf::Vector2u updatedSize);

    void update(InputHandler* inputHandler, sf::Vector2i pos);

    void draw(sf::RenderWindow& window) const;

    bool isClicked() const;

    bool isJustClicked() const;

    void updateColour(sf::Color color);
};



#endif //TEXTBUTTON_H
