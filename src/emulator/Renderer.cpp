#include "Renderer.h"

#include <iostream>

void Renderer::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (uint8_t y = 0; y < this->rows; ++y) {
        for (uint8_t x = 0; x < this->columns; ++x) {
            if (this->display[y * this->columns + x]) {
                const SDL_FRect rect = {x * this->scale, y * this->scale, this->scale, this->scale};
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

bool Renderer::setPixel(const uint8_t x, const uint8_t y) {
    const uint16_t pixelLoc = y * this->columns + x;
    const bool oldValue = this->display[pixelLoc];
    this->display[pixelLoc] = !oldValue;

    return oldValue;
}

bool Renderer::getPixel(const uint8_t x, const uint8_t y) {
    return this->display[y * this-> columns + x];
}

void Renderer::clear() {
    this->display.assign(this->display.size(), false);
}

uint8_t Renderer::getColumns() const {
    return this->columns;
}

uint8_t Renderer::getRows() const {
    return this->rows;
}

float Renderer::getScale() const {
    return this->scale;
}