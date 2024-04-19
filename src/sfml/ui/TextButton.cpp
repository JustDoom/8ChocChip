#include "TextButton.h"

#include "../../util/MiscUtil.h"

TextButton::TextButton(float x, float y, float width, float height, std::string buttonText, sf::Font* font) {
    this->button.setSize(sf::Vector2f(width, height));
    this->button.setPosition(sf::Vector2f(x, y));

    this->originalPosition = this->button.getPosition();
    this->originalSize = this->button.getSize();

    this->text.setFont(*font);
    this->text.setString(buttonText);
    this->text.setCharacterSize(20);
    this->text.setFillColor(sf::Color::Black);
    this->text.setPosition(x + width / 2 - this->text.getGlobalBounds().width / 2, y + height / 2 - this->text.getGlobalBounds().height / 2);

    this->idleColor = sf::Color(192, 192, 192);
    this->hoverColor = sf::Color(128, 128, 128);
    this->activeColor = sf::Color(64, 64, 64);

    this->button.setFillColor(this->idleColor);

    this->isPressed = false;
    this->isHovered = false;
}

void TextButton::updateSize(const sf::Vector2u originalSize, const sf::Vector2u updatedSize) {
    this->button.setSize(sf::Vector2f(this->originalSize.x / originalSize.x * updatedSize.x, this->originalSize.y / originalSize.y * updatedSize.y));
    this->button.setPosition(this->originalPosition.x / originalSize.x * updatedSize.x, this->originalPosition.y / originalSize.y * updatedSize.y);

    this->text.setPosition(this->button.getPosition().x + this->button.getSize().x / 2 - this->text.getGlobalBounds().width / 2, this->button.getPosition().y + this->button.getSize().y / 2 - this->text.getGlobalBounds().height / 2);
}

void TextButton::update(InputHandler* inputHandler, sf::Vector2i pos) {
    this->lastPressed = this->isPressed;
    this->isHovered = this->button.getGlobalBounds().contains(pos.x, pos.y);

    if (this->isHovered && inputHandler->isJustClicked(sf::Mouse::Left)) {
        this->isPressed = true;
        updateColour(this->activeColor);
    } else if (this->isHovered && inputHandler->isPressed(sf::Keyboard::Key::LShift)) {
        updateColour( sf::Color(255, 0, 0));
    } else if (this->isHovered) {
        updateColour(this->hoverColor);
    } else {
        this->isPressed = false;
        updateColour(this->idleColor);
    }
}

void TextButton::updateColour(sf::Color color) {
    if (this->color == color) {
        return;
    }

    this->button.setFillColor(color);
}

void TextButton::draw(sf::RenderWindow &window) {
    window.draw(this->button);
    window.draw(this->text);
}

bool TextButton::isClicked() {
    return this->isPressed;
}

bool TextButton::isJustClicked() {
    return !this->lastPressed && this->isPressed;
}