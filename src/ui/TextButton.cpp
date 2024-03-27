//
// Created by doom on 3/27/24.
//

#include "TextButton.h"

TextButton::TextButton(float x, float y, float width, float height, std::string buttonText) {
    button.setSize(sf::Vector2f(width, height));
    button.setPosition(sf::Vector2f(x, y));

    font.loadFromFile("../assets/font.ttf"); // Load your font here
    text.setFont(font);
    text.setString(buttonText);
    text.setCharacterSize(24); // Set your text size
    text.setFillColor(sf::Color::Black);
    text.setPosition(x + width / 4, y + height / 4); // Adjust text position

    idleColor = sf::Color(192, 192, 192); // Grey
    hoverColor = sf::Color(128, 128, 128); // Dark Grey
    activeColor = sf::Color(64, 64, 64); // Darker Grey

    button.setFillColor(idleColor);

    isPressed = false;
    isHovered = false;
}

void TextButton::updateSize(float width, float height) {
    button.setSize(sf::Vector2f(width, height));
}

void TextButton::update(sf::RenderWindow &window) {
    isHovered = button.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);

    if (isHovered && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        isPressed = true;
        button.setFillColor(activeColor);
    } else if (isHovered) {
        button.setFillColor(hoverColor);
    } else {
        isPressed = false;
        button.setFillColor(idleColor);
    }
}

void TextButton::draw(sf::RenderWindow &window) {
    window.draw(button);
    window.draw(text);
}

bool TextButton::isClicked() {
    return isPressed;
}
