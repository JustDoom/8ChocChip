#include "TextButton.h"

#include <iostream>

TextButton::TextButton(const float x, const float y, const float width, const float height, TTF_Text* text, const std::weak_ptr<ChildHolderElement>& parent, SDL_Color idleColour) :
        Element(parent), text(text), idleColor(idleColour), hoverColor(SDL_Color{128, 128, 128, 255}),
        activeColor(SDL_Color{64, 64, 64, 255}), deleteColor(SDL_Color{255, 0, 0, 255}),
        localPosition({x, y}) {

    const auto lock = parent.lock();
    this->buttonBox = SDL_FRect{lock->getX() + x, lock->getY() + y, width, height};

    TTF_SetTextColor(text, 0, 0, 0, 255);
    SDL_Point textSize{};
    TTF_GetTextSize(text, &textSize.x, &textSize.y);
    this->textPosition.x = this->buttonBox.x + this->buttonBox.w / 2 - textSize.x / 2;
    this->textPosition.y = this->buttonBox.y + this->buttonBox.h / 2 - textSize.y / 2 + 4;

    this->currentColor = this->idleColor;
}

void TextButton::updateSize(const SDL_Point& originalSize, const SDL_Point& updatedSize) {

}

void TextButton::update(InputHandler& inputHandler) {
    SDL_FPoint pos{};
    SDL_GetMouseState(&pos.x, &pos.y);

    this->lastPressed = this->isPressed;
    this->hovered = SDL_PointInRectFloat(&pos, &this->buttonBox); // TODO: Can hover over 2 at once sometimes and open 2
    if (const auto lock = this->parent.lock()) {
        this->hovered = this->hovered && lock->isPointInsideArea(pos);
    }

    if (inputHandler.isJustClicked(SDL_BUTTON_LEFT)) {
        this->isPressed = true;
        updateColour(this->activeColor);
        if (this->onClick) {
            this->onClick();
        }
    }

    // if (button->isHovered()) {
    //     if (inputHandler.isPressed(SDL_SCANCODE_LSHIFT)) {
    //         updateColour(this->deleteColor);
    //         if (inputHandler.isJustClicked(SDL_BUTTON_LEFT)) {
    //             button.destroy();
    //         }
    //     } else if (inputHandler.isJustClicked(SDL_BUTTON_LEFT)) {
    //         this->isPressed = true;
    //         updateColour(this->activeColor);
    //         if (this->onClick) {
    //             this->onClick();
    //         }
    //     } else {
    //         updateColour(this->hoverColor);
    //     }
    // } else {
    //     this->isPressed = false;
    //     updateColour(this->idleColor);
    // }
}

void TextButton::updateColour(const SDL_Color& colour) {
    this->currentColor = colour;
}

void TextButton::setIdleColour(const SDL_Color& colour) {
    this->idleColor = colour;
}

void TextButton::draw(SDL_Renderer* window) {
    SDL_SetRenderDrawColor(window, this->currentColor.r, this->currentColor.g, this->currentColor.b, this->currentColor.a);
    SDL_RenderFillRect(window, &this->buttonBox);
    if (!TTF_DrawRendererText(this->text, this->textPosition.x, this->textPosition.y)) {
        SDL_Log("Text rendering failed: %s\n", SDL_GetError());
    }
}

bool TextButton::isClicked() const {
    return this->isPressed;
}

bool TextButton::isJustClicked() const {
    return !this->lastPressed && this->isPressed;
}

bool TextButton::isHovered() const {
    return this->hovered;
}

SDL_FRect& TextButton::getRect() {
    return this->buttonBox;
}

float TextButton::getX() {
    return this->localPosition.x;
}

float TextButton::getY() {
    return this->localPosition.y;
}

float TextButton::getRealX() {
    return this->buttonBox.x;
}

float TextButton::getRealY() {
    return this->buttonBox.y;
}

float TextButton::getWidth() {
    return this->buttonBox.w;
}

float TextButton::getHeight() {
    return this->buttonBox.h;
}

void TextButton::setX(float x) {
    this->localPosition.x = x;
    setRealX(this->parent.lock()->getX() + x);
}

void TextButton::setY(float y) {
    this->localPosition.y = y;
    setRealY(this->parent.lock()->getY() + y);
}

void TextButton::setRealX(float x) {
    this->buttonBox.x = x;
    TTF_SetTextColor(this->text, 0, 0, 0, 255);
    SDL_Point textSize{};
    TTF_GetTextSize(this->text, &textSize.x, &textSize.y);
    this->textPosition.x = this->buttonBox.x + this->buttonBox.w / 2 - textSize.x / 2;
    this->textPosition.y = this->buttonBox.y + this->buttonBox.h / 2 - textSize.y / 2 + 4;
}

void TextButton::setRealY(float y) {
    this->buttonBox.y = y;
    TTF_SetTextColor(this->text, 0, 0, 0, 255);
    SDL_Point textSize{};
    TTF_GetTextSize(this->text, &textSize.x, &textSize.y);
    this->textPosition.x = this->buttonBox.x + this->buttonBox.w / 2 - textSize.x / 2;
    this->textPosition.y = this->buttonBox.y + this->buttonBox.h / 2 - textSize.y / 2 + 4;
}

bool TextButton::isPointInsideArea(SDL_FPoint& point) {
    return SDL_PointInRectFloat(&point, &this->buttonBox);
}

void TextButton::setOnClick(const std::function<void()> function) {
    this->onClick = function;
}