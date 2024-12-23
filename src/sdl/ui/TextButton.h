#ifndef TEXTBUTTON_H
#define TEXTBUTTON_H

#include "../InputHandler.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class TextButton {
private:
    SDL_FRect button;
    SDL_FPoint originalSize;
    SDL_FPoint originalPosition;
    TTF_Text* text;
    SDL_FPoint textPos;
    SDL_Color idleColor;
    SDL_Color hoverColor;
    SDL_Color activeColor;
    SDL_Color deleteColor;
    SDL_Color currentColor;
    bool isPressed;
    bool lastPressed{};
    bool isHovered;

public:
    TextButton(float x, float y, float width, float height, TTF_Text* text);

    void updateSize(const SDL_Point & originalSize, const SDL_Point & updatedSize);

    void update(InputHandler& inputHandler, const SDL_FPoint & pos);

    void draw(SDL_Renderer* window) const;

    bool isClicked() const;

    bool isJustClicked() const;

    void updateColour(const SDL_Color & color);
};

#endif