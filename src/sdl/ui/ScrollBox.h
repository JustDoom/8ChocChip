#ifndef SCROLLBOX_H
#define SCROLLBOX_H

#include "Element.h"

#include <memory>
#include <utility>
#include <vector>

class ScrollBox : public Element {
private:
    std::vector<std::shared_ptr<Element>> elements;
    SDL_FRect box;
    float scrollPosition = 0;
    float scrollSpeed = 20;
public:
    ScrollBox(float x, float y, float width, float height) : ScrollBox(x, y, width, height, std::weak_ptr<Element>()) {}
    ScrollBox(float x, float y, float width, float height, const std::weak_ptr<Element>& parent) :
        Element(parent), elements(std::move(elements)), box({x, y, width, height}) {}

    void draw(SDL_Renderer *window) override;
    void update(InputHandler &inputHandler) override;
    bool handleEvent(SDL_Event &event) override;

    float getX() override;
    float getY() override;
    float getRealX() override;
    float getRealY() override;
    float getWidth() override;
    float getHeight() override;

    void setElements(std::vector<std::shared_ptr<Element>> elements);
};

#endif