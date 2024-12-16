#include "TextButton.h"

#include <iostream>

TextButton::TextButton(float x, float y, float width, float height, TTF_Text* text) :
        text(text), idleColor(SDL_Color{192, 192, 192, 255}),
        hoverColor(SDL_Color{128, 128, 128, 255}), activeColor(SDL_Color{64, 64, 64, 255}),
        deleteColor(SDL_Color{255, 0, 0, 255}) {
    this->button = SDL_FRect{x, y, width, height};

    currentColor = idleColor;

    TTF_SetTextColor(text, 0, 0, 0, 255);
    SDL_Point textSize{};
    TTF_GetTextSize(text, &textSize.x, &textSize.y);
    this->textPos.x = x + width / 2 - textSize.x / 2;
    this->textPos.y = y + height / 2 - textSize.y / 2 + 4;

    this->originalPosition = {button.x, button.y};
    this->originalSize = {button.w, button.h};

    this->isPressed = false;
    this->isHovered = false;
}

void TextButton::updateSize(const SDL_Point& originalSize, const SDL_Point& updatedSize) {
    this->button.x = this->originalPosition.x / originalSize.x * updatedSize.x;
    this->button.y = this->originalPosition.y / originalSize.y * updatedSize.y;
    this->button.w = this->originalSize.x / originalSize.x * updatedSize.x;
    this->button.h = this->originalSize.y / originalSize.y * updatedSize.y;

    SDL_Point textSize{};
    TTF_GetTextSize(text, &textSize.x, &textSize.y);
    this->textPos.x = this->button.x + this->button.w / 2 - textSize.x / 2;
    this->textPos.y = this->button.y + this->button.h / 2 - textSize.y / 2 + 4;
}

void TextButton::update(InputHandler& inputHandler, SDL_FPoint& pos) {
    this->lastPressed = this->isPressed;
    this->isHovered = SDL_PointInRectFloat(&pos, &this->button);

    if (this->isHovered && inputHandler.isJustClicked(SDL_BUTTON_LEFT)) {
        this->isPressed = true;
        updateColour(this->activeColor);
    } else if (this->isHovered && inputHandler.isPressed(SDL_SCANCODE_LSHIFT)) {
        updateColour(this->deleteColor);
    } else if (this->isHovered) {
        updateColour(this->hoverColor);
    } else {
        this->isPressed = false;
        updateColour(this->idleColor);
    }
}

void TextButton::updateColour(SDL_Color& color) {
    // if (this->currentColor == color) {
    //     return;
    // }

    this->currentColor = color;
}

void TextButton::draw(SDL_Renderer* window) const {
    SDL_SetRenderDrawColor(window, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
    SDL_RenderFillRect(window, &this->button);
    if (!TTF_DrawRendererText(text, textPos.x, textPos.y)) {
        SDL_Log("Text rendering failed: %s\n", SDL_GetError());
    }
}

bool TextButton::isClicked() const {
    return this->isPressed;
}

bool TextButton::isJustClicked() const {
    return !this->lastPressed && this->isPressed;
}