#include "MainBox.h"

void MainBox::draw(SDL_Renderer* window) {
    SDL_SetRenderDrawColor(window, 255, 255, 255, 255);
    SDL_RenderFillRect(window, &this->box);

    // const SDL_Rect clipRect = {static_cast<int>(this->box.x), static_cast<int>(this->box.y), static_cast<int>(this->box.w), static_cast<int>(this->box.h)};
     // SDL_SetRenderClipRect(window, &clipRect);

    for (const std::shared_ptr<Element>& element : this->elements) {
    //     SDL_FRect area = {element->getX() + getX(), element->getY() + getY(), element->getWidth(), element->getHeight()};
    //     if (SDL_HasRectIntersectionFloat(&area, &this->box)) {
            element->draw(window);
        // }
    }
    // SDL_SetRenderClipRect(window, nullptr);
}

void MainBox::update(InputHandler& inputHandler) {
    for (const std::shared_ptr<Element>& element : std::vector(this->elements)) {
        element->update(inputHandler);
    }
}

bool MainBox::handleEvent(SDL_Event& event) {
    for (const std::shared_ptr<Element>& element : this->elements) {
        element->handleEvent(event);
    }

    return true;
}

void MainBox::removeElement(Element *element) {
    ChildHolderElement::removeElement(element);
}

float MainBox::getX() {
    return this->box.x;
}

float MainBox::getY() {
    return this->box.y;
}

float MainBox::getRealX() {
    return this->box.x;
}

float MainBox::getRealY() {
    return this->box.y;
}

float MainBox::getWidth() {
    return this->box.w;
}

float MainBox::getHeight() {
    return this->box.h;
}

void MainBox::setX(float x) {
    this->box.x = x;
}

void MainBox::setY(float y) {
    this->box.y = y;
}

void MainBox::setRealX(float x) {

}

void MainBox::setRealY(float y) {

}

bool MainBox::isPointInsideArea(SDL_FPoint &point) {
    return SDL_PointInRectFloat(&point, &this->box);
}