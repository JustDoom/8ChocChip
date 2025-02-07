#include "TextButton.h"

#include <iostream>

TextButton::TextButton(const float x, const float y, const float width, const float height, TTF_Text *text, const std::weak_ptr<Element>& parent, SDL_Color idleColour) :
        Element(parent), text(text), idleColor(idleColour),
        hoverColor(SDL_Color{128, 128, 128, 255}), activeColor(SDL_Color{64, 64, 64, 255}),
        deleteColor(SDL_Color{255, 0, 0, 255}) {

    this->originalButton = SDL_FRect{x, y, width, height};
    this->renderButton = this->originalButton;

    this->currentColor = this->idleColor;

    TTF_SetTextColor(text, 0, 0, 0, 255);
    SDL_Point textSize{};
    TTF_GetTextSize(text, &textSize.x, &textSize.y);
    this->originalTextPos.x = x + width / 2 - textSize.x / 2;
    this->originalTextPos.y = y + height / 2 - textSize.y / 2 + 4;
    this->renderTextPos = this->originalTextPos;

    this->originalPosition = {this->originalButton.x, this->originalButton.y};
    this->originalSize = {this->originalButton.w, this->originalButton.h};

    this->isPressed = false;
    this->isHovered = false;
}

void TextButton::updateSize(const SDL_Point& originalSize, const SDL_Point& updatedSize) {
    this->originalButton.x = this->originalPosition.x / originalSize.x * updatedSize.x;
    this->originalButton.y = this->originalPosition.y / originalSize.y * updatedSize.y;
    this->originalButton.w = this->originalSize.x / originalSize.x * updatedSize.x;
    this->originalButton.h = this->originalSize.y / originalSize.y * updatedSize.y;

    SDL_Point textSize{};
    TTF_GetTextSize(this->text, &textSize.x, &textSize.y);
    this->originalTextPos.x = this->originalButton.x + this->originalButton.w / 2 - textSize.x / 2;
    this->originalTextPos.y = this->originalButton.y + this->originalButton.h / 2 - textSize.y / 2 + 4;
}

void TextButton::update(InputHandler& inputHandler) {
    SDL_FPoint pos{};
    SDL_GetMouseState(&pos.x, &pos.y);

    this->lastPressed = this->isPressed;
    this->isHovered = SDL_PointInRectFloat(&pos, &this->renderButton); // TODO: Can hover over 2 at once sometimes and open 2
    if (const auto lock = this->parent.lock()) {
        this->isHovered = this->isHovered && lock->isPointInsideArea(pos);
    }

    if (this->isHovered && inputHandler.isJustClicked(SDL_BUTTON_LEFT)) {
        this->isPressed = true;
        updateColour(this->activeColor);
        if (this->onClick) {
            this->onClick();
        }
    } else if (this->isHovered && inputHandler.isPressed(SDL_SCANCODE_LSHIFT)) {
        updateColour(this->deleteColor);
    } else if (this->isHovered) {
        updateColour(this->hoverColor);
    } else {
        this->isPressed = false;
        updateColour(this->idleColor);
    }
}

void TextButton::updateColour(const SDL_Color& colour) {
    this->currentColor = colour;
}

void TextButton::setIdleColour(const SDL_Color& colour) {
    this->idleColor = colour;
}

void TextButton::draw(SDL_Renderer* window) {
    SDL_SetRenderDrawColor(window, this->currentColor.r, this->currentColor.g, this->currentColor.b, this->currentColor.a);
    if (const auto lock = this->parent.lock()) {
        this->renderButton.y = this->originalButton.y + lock->getY();
        this->renderTextPos.y = this->originalTextPos.y + lock->getY();
    }
    SDL_RenderFillRect(window, &this->renderButton);
    if (!TTF_DrawRendererText(this->text, this->renderTextPos.x, this->renderTextPos.y)) {
        SDL_Log("Text rendering failed: %s\n", SDL_GetError());
    }
}

bool TextButton::isClicked() const {
    return this->isPressed;
}

bool TextButton::isJustClicked() const {
    return !this->lastPressed && this->isPressed;
}

SDL_FRect& TextButton::getRect() {
    return this->originalButton;
}

float TextButton::getX() {
    return this->originalButton.x;
}

float TextButton::getY() {
    return this->originalButton.y;
}

float TextButton::getRealX() {
    return this->originalButton.x;
}

float TextButton::getRealY() {
    return this->originalButton.y;
}

float TextButton::getWidth() {
    return this->originalButton.w;
}

float TextButton::getHeight() {
    return this->originalButton.h;
}

bool TextButton::isPointInsideArea(SDL_FPoint& point) {
    return SDL_PointInRectFloat(&point, &this->renderButton);
}

void TextButton::setOnClick(const std::function<void()> function) {
    this->onClick = function;
}