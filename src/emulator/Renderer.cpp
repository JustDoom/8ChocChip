//
// Created by doom on 19/03/24.
//

#include "Renderer.h"

Renderer::Renderer() {
    this->scale = 15; // Scale up because 64 x 32 would be tiny on our screens now
    this->display.resize(this->columns * this->rows);
}

void Renderer::render(sf::RenderWindow* window) {
    // Render the display
    for (uint16_t y = 0; y < this->rows; ++y) {
        for (uint16_t x = 0; x < this->columns; ++x) {
            if (this->display[y * this->columns + x]) {
                drawRectangle(*window, x * this->scale, y * this->scale, this->scale, this->scale);
            }
        }
    }
}

bool Renderer::setPixel(uint8_t x, uint16_t y) {
    // Wrap around if x or y is out of bounds
    x %= this->columns;
    y %= this->rows;

    uint16_t pixelLoc = x + (y * this->columns);
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

void Renderer::drawRectangle(sf::RenderWindow &window, int x, int y, int width, int height) {
    sf::RectangleShape rectangle(sf::Vector2f(width, height));
    rectangle.setPosition(x, y);
    rectangle.setFillColor(sf::Color::White);
    window.draw(rectangle);
}

uint16_t Renderer::getColumns() const {
    return this->columns;
}

uint16_t Renderer::getRows() const {
    return this->rows;
}

uint8_t Renderer::getScale() const {
    return this->scale;
}
