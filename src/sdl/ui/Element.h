#ifndef ELEMENT_H
#define ELEMENT_H

#include <memory>

#include <SDL3/SDL.h>

#include "../InputHandler.h"

class Element {
protected:
    std::weak_ptr<Element> parent;
public:
    explicit Element();
    explicit Element(const std::weak_ptr<Element>& parent) : parent(parent) {}
    virtual ~Element() = default;
    virtual void update(InputHandler& inputHandler) = 0;
    virtual void draw(SDL_Renderer* window) = 0;
    virtual bool handleEvent(SDL_Event &event) = 0;

    virtual float getX() = 0;
    virtual float getY() = 0;
    virtual float getRealX() = 0; // Without any possible extra processing
    virtual float getRealY() = 0;
    virtual float getWidth() = 0;
    virtual float getHeight() = 0;

    virtual bool isPointInsideArea(SDL_FPoint& point) = 0;
};

#endif
