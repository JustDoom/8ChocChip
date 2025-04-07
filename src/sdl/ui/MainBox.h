#ifndef MAINBOX_H
#define MAINBOX_H

#include "ChildHolderElement.h"

class MainBox : public ChildHolderElement {
private:
    SDL_FRect box;
public:
    MainBox(const float width, const float height) : ChildHolderElement(std::weak_ptr<ChildHolderElement>()), box({0, 0, width, height}) {}

    void draw(SDL_Renderer *window) override;
    void update(InputHandler &inputHandler) override;
    bool handleEvent(SDL_Event &event) override;

    void removeElement(Element *element) override;

    float getX() override;
    float getY() override;
    float getRealX() override;
    float getRealY() override;
    float getWidth() override;
    float getHeight() override;
    void setX(float x) override;
    void setY(float y) override;
    void setRealX(float x) override;
    void setRealY(float y) override;

    bool isPointInsideArea(SDL_FPoint& point) override;
};

#endif
