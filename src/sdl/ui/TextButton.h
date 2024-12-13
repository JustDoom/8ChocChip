#ifndef TEXTBUTTON_H
#define TEXTBUTTON_H

#include "../InputHandler.h"
#include <string>

#include <SDL3/SDL.h>

class TextButton {
private:
    SDL_FRect button;
    SDL_Point originalSize;
    SDL_Point originalPosition;
    std::string& text;
    // sf::Font* font{};
    SDL_Color color;
    SDL_Color idleColor;
    SDL_Color hoverColor;
    SDL_Color activeColor;
    SDL_Color currentColor;
    bool isPressed;
    bool lastPressed{};
    bool isHovered;

public:
    TextButton(float x, float y, float width, float height, const std::string& buttonText);

    void updateSize(SDL_Point originalSize, SDL_Point updatedSize);

    void update(InputHandler& inputHandler, SDL_FPoint pos);

    void draw(SDL_Renderer* window) const;

    bool isClicked() const;

    bool isJustClicked() const;

    void updateColour(SDL_Color color);
};



#endif