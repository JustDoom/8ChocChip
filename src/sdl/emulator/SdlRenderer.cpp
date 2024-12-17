#include "SdlRenderer.h"

void SdlRenderer::drawPixel(SDL_Renderer* renderer, const uint16_t x, const uint16_t y) {
    const SDL_FRect rect = {x * this->scale, y * this->scale, this->scale, this->scale};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Set color to white
    SDL_RenderFillRect(renderer, &rect);
}

float SdlRenderer::getScale() const {
    return this->scale;
}