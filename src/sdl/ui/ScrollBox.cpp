#include "ScrollBox.h"

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
    const int size = this->elements.size();
    for (const std::shared_ptr<Element>& element : std::vector(this->elements)) {
        element->update(inputHandler);
    }
    if (size != this->elements.size()) {
        int count = 0;
        for (const std::shared_ptr<Element>& element : this->elements) {
            element->setY(25.0f * count++);
        }
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

void ScrollBox::removeElement(Element *element) {
    ChildHolderElement::removeElement(element);
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

void ScrollBox::setX(float x) {
    this->box.x = x;
}

void ScrollBox::setY(float y) {
    this->box.y = y;
}

bool ScrollBox::isPointInsideArea(SDL_FPoint &point) {
    return SDL_PointInRectFloat(&point, &this->box);
}