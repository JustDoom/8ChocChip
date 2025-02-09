#ifndef SCROLLBOX_H
#define SCROLLBOX_H

#include <memory>

#include "ChildHolderElement.h"

class ScrollBox : public ChildHolderElement {
private:
    SDL_FRect box;
    float scrollPosition = 0;
    float scrollSpeed = 20;
public:
    ScrollBox(float x, float y, float width, float height) : ScrollBox(x, y, width, height, std::weak_ptr<ChildHolderElement>()) {}
    ScrollBox(float x, float y, float width, float height, const std::weak_ptr<ChildHolderElement>& parent) :
        ChildHolderElement(parent), box({x, y, width, height}) {}

    void draw(SDL_Renderer *window) override;
    void update(InputHandler &inputHandler) override;
    bool handleEvent(SDL_Event &event) override;

    float getX() override;
    float getY() override;
    float getRealX() override;
    float getRealY() override;
    float getWidth() override;
    float getHeight() override;
    void setX(float x) override;
    void setY(float y) override;

    bool isPointInsideArea(SDL_FPoint& point) override;
};

#endif