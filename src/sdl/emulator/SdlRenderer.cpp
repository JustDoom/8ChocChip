#include "SdlRenderer.h"

#include <iostream>
#include <ostream>

void SdlRenderer::drawPixel(SDL_Renderer* renderer, uint16_t x, uint16_t y) {
    const SDL_FRect rect = {x * scale, y * scale, scale, scale};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Set color to white
    SDL_RenderFillRect(renderer, &rect);
}

float SdlRenderer::getScale() const {
    return this->scale;
}