#ifndef TEXTBUTTON_H
#define TEXTBUTTON_H

#include <functional>


#include "../InputHandler.h"
#include "Element.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class TextButton : public Element {
private:
    SDL_FRect originalButton;
    SDL_FRect renderButton;
    SDL_FPoint originalSize;
    SDL_FPoint originalPosition;
    TTF_Text* text;
    SDL_FPoint originalTextPos;
    SDL_FPoint renderTextPos;
    SDL_Color idleColor;
    SDL_Color hoverColor;
    SDL_Color activeColor;
    SDL_Color deleteColor;
    SDL_Color currentColor;
    bool isPressed;
    bool lastPressed{};
    bool isHovered;

    std::function<void()> onClick;

public:
    TextButton(float x, float y, float width, float height, TTF_Text* text) : TextButton(x, y, width, height, text, std::weak_ptr<Element>()) {}
    TextButton(float x, float y, float width, float height, TTF_Text* text, const std::weak_ptr<Element>& parent) : TextButton(x, y, width, height, text, parent, SDL_Color{192, 192, 192, 255}) {}
    TextButton(float x, float y, float width, float height, TTF_Text* text, SDL_Color idleColour) : TextButton(x, y, width, height, text, std::weak_ptr<Element>(), idleColour) {}
    TextButton(float x, float y, float width, float height, TTF_Text* text, const std::weak_ptr<Element>& parent, SDL_Color idleColour);

    void updateSize(const SDL_Point & originalSize, const SDL_Point & updatedSize);

    void update(InputHandler& inputHandler) override;

    void draw(SDL_Renderer* window) override;

    bool handleEvent(SDL_Event &event) override { return true; };

    bool isClicked() const;

    bool isJustClicked() const;

    void updateColour(const SDL_Color& colour);
    void setIdleColour(const SDL_Color& colour);

    SDL_FRect& getRect();

    float getX() override;
    float getY() override;
    float getRealX() override;
    float getRealY() override;
    float getWidth() override;
    float getHeight() override;

    void setOnClick(const std::function<void()> function);
};

#endif