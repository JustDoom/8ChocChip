#include "Renderer.h"

Renderer::Renderer() {
    this->display.resize(this->columns * this->rows);
}

void Renderer::render(SDL_Renderer* renderer) {
    // Render the display
    for (uint16_t y = 0; y < this->rows; ++y) {
        for (uint16_t x = 0; x < this->columns; ++x) {
            if (this->display[y * this->columns + x]) {
                drawPixel(renderer, x, y);
            }
        }
    }
}

bool Renderer::setPixel(uint8_t x, uint16_t y) {
    // Wrap around if x or y is out of bounds
    // x %= this->columns;
    // y %= this->rows;

    const uint16_t pixelLoc = x + (y * this->columns);
    this->display[pixelLoc] = !this->display[pixelLoc];

    return !this->display[pixelLoc];
}

void Renderer::clear() {
    for (uint16_t y = 0; y < this->rows; ++y) {
        for (uint16_t x = 0; x < this->columns; ++x) {
            if (this->display[y * this->columns + x]) {
                setPixel(x, y);
            }
        }
    }
}

uint16_t Renderer::getColumns() const {
    return this->columns;
}

uint16_t Renderer::getRows() const {
    return this->rows;
}