#include "SdlRenderer.h"

SdlRenderer::SdlRenderer(SDL_Renderer* window) {
    this->window = window;

    this->scale = 15; // Scale up because 64 x 32 would be tiny on our screens now
}

void SdlRenderer::drawPixel(uint16_t x, uint16_t y) {
    SDL_Rect rect = {x * scale, y * scale, scale, scale};
    SDL_SetRenderDrawColor(window, 255, 255, 255, 255); // Set color to white
    SDL_RenderFillRect(window, &rect);
}

uint8_t SdlRenderer::getScale() const {
    return this->scale;
}