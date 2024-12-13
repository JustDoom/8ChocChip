#include "TextButton.h"

#include <iostream>

TextButton::TextButton(float x, float y, float width, float height, const std::string& buttonText) : text(text) {
    this->button = SDL_FRect{x, y, width, height};
    // this->button.setPosition(sf::Vector2f(x, y));

    // this->originalPosition = this->button.getPosition();
    // this->originalSize = this->button.getSize();

    // this->text.setFont(*font);
    // this->text.setString(buttonText);
    // this->text.setCharacterSize(20);
    // this->text.setFillColor(sf::Color::Black);
    // this->text.setPosition(x + width / 2 - this->text.getGlobalBounds().width / 2, y + height / 2 - this->text.getGlobalBounds().height / 2);

    this->idleColor = SDL_Color{192, 192, 192};
    this->hoverColor = SDL_Color{128, 128, 128};
    this->activeColor = SDL_Color{64, 64, 64};

    // this->button.setFillColor(this->idleColor);

    this->isPressed = false;
    this->isHovered = false;
}

void TextButton::updateSize(const SDL_Point originalSize, const SDL_Point updatedSize) {
    // this->button.setSize(SDL_Point(this->originalSize.x / originalSize.x * updatedSize.x, this->originalSize.y / originalSize.y * updatedSize.y));
    // this->button.setPosition(this->originalPosition.x / originalSize.x * updatedSize.x, this->originalPosition.y / originalSize.y * updatedSize.y);

    // this->text.setPosition(this->button.getPosition().x + this->button.getSize().x / 2 - this->text.getGlobalBounds().width / 2, this->button.getPosition().y + this->button.getSize().y / 2 - this->text.getGlobalBounds().height / 2);
}

void TextButton::update(InputHandler& inputHandler, SDL_FPoint pos) {
    this->lastPressed = this->isPressed;
    this->isHovered = SDL_PointInRectFloat(&pos, &this->button);

    if (this->isHovered && inputHandler.isJustClicked(SDL_BUTTON_LEFT)) {
        this->isPressed = true;
        updateColour(this->activeColor);
    } else if (this->isHovered && inputHandler.isPressed(SDL_KMOD_LSHIFT)) {
        updateColour( SDL_Color{255, 0, 0});
    } else if (this->isHovered) {
        updateColour(this->hoverColor);
    } else {
        this->isPressed = false;
        updateColour(this->idleColor);
    }
}

void TextButton::updateColour(const SDL_Color color) {
    if (this->color.r == color.r && this->color.g == color.g && this->color.b == color.b) {
        return;
    }

    this->currentColor = color;
}

void TextButton::draw(SDL_Renderer* window) const {
    // window.draw(this->button);

    SDL_SetRenderDrawColor(window, this->currentColor.r, this->currentColor.g, this->currentColor.b, this->currentColor.a);
    SDL_RenderFillRect(window, &this->button);
}

bool TextButton::isClicked() const {
    return this->isPressed;
}

bool TextButton::isJustClicked() const {
    return !this->lastPressed && this->isPressed;
}