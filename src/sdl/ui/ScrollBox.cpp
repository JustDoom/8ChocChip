#include "ScrollBox.h"

#include <utility>

void ScrollBox::draw(SDL_Renderer* window) {
    SDL_SetRenderDrawColor(window, 200, 200, 200, 255);
    SDL_RenderFillRect(window, &this->box);

    const SDL_Rect clipRect = {static_cast<int>(this->box.x), static_cast<int>(this->box.y), static_cast<int>(this->box.w), static_cast<int>(this->box.h)};
    SDL_SetRenderClipRect(window, &clipRect);

    for (const std::shared_ptr<Element>& element : this->elements) {
        SDL_FRect area = {element->getX() + getX(), element->getY() + getY(), element->getWidth(), element->getHeight()};
        if (SDL_HasRectIntersectionFloat(&area, &this->box)) {
            element->draw(window);
        }
    }
    SDL_SetRenderClipRect(window, nullptr);
}

void ScrollBox::update(InputHandler& inputHandler) {
    for (const std::shared_ptr<Element>& element : this->elements) {
        element->update(inputHandler);
    }
}

bool ScrollBox::handleEvent(SDL_Event& event) {
    switch (event.type) {
        case SDL_EVENT_MOUSE_WHEEL:
            SDL_FPoint pos{};
            SDL_GetMouseState(&pos.x, &pos.y);
            if (!SDL_PointInRectFloat(&pos, &this->box) || this->elements.size() * 25 < getHeight()) {
                break;
            }
            this->scrollPosition -= event.wheel.y * this->scrollSpeed;
            if (this->scrollPosition < 0) {
                this->scrollPosition = 0;
            }
            if (float position = static_cast<float>(this->elements.size()) * 25.0f - getHeight();
                this->scrollPosition > position) {
                this->scrollPosition = position;
            }
        break;
    }

    for (const std::shared_ptr<Element>& element : this->elements) {
        element->handleEvent(event);
    }

    return true;
}

float ScrollBox::getX() {
    return this->box.x;
}

float ScrollBox::getY() {
    return this->box.y - this->scrollPosition;
}

float ScrollBox::getRealX() {
    return this->box.x;
}

float ScrollBox::getRealY() {
    return this->box.y;
}

float ScrollBox::getWidth() {
    return this->box.w;
}

float ScrollBox::getHeight() {
    return this->box.h;
}

void ScrollBox::setElements(std::vector<std::shared_ptr<Element>> elements) {
    this->elements = std::move(elements);
}
