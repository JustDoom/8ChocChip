#ifndef TEXTBUTTON_H
#define TEXTBUTTON_H

#include <functional>


#include "../InputHandler.h"
#include "ChildHolderElement.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class TextButton : public Element {
private:
    SDL_FPoint localPosition;
    SDL_FRect buttonBox;
    TTF_Text* text;
    SDL_FPoint textPosition;
    SDL_Color idleColor;
    SDL_Color hoverColor;
    SDL_Color activeColor;
    SDL_Color deleteColor;
    SDL_Color currentColor;
    bool isPressed = false;
    bool hovered = false;
    bool lastPressed = false;

    std::function<void()> onClick;

public:
    TextButton(float x, float y, float width, float height, TTF_Text* text) : TextButton(x, y, width, height, text, std::weak_ptr<ChildHolderElement>()) {}
    TextButton(float x, float y, float width, float height, TTF_Text* text, const std::weak_ptr<ChildHolderElement>& parent) : TextButton(x, y, width, height, text, parent, SDL_Color{192, 192, 192, 255}) {}
    TextButton(float x, float y, float width, float height, TTF_Text* text, SDL_Color idleColour) : TextButton(x, y, width, height, text, std::weak_ptr<ChildHolderElement>(), idleColour) {}
    TextButton(float x, float y, float width, float height, TTF_Text* text, const std::weak_ptr<ChildHolderElement>& parent, SDL_Color idleColour);

    void updateSize(const SDL_Point & originalSize, const SDL_Point & updatedSize);
    void update(InputHandler& inputHandler) override;
    void draw(SDL_Renderer* window) override;
    bool handleEvent(SDL_Event &event) override { return true; };

    bool isClicked() const;
    bool isJustClicked() const;
    bool isHovered() const;

    void updateColour(const SDL_Color& colour);
    void setIdleColour(const SDL_Color& colour);

    SDL_FRect& getRect();

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

    void setOnClick(const std::function<void()> function);
};

#endif