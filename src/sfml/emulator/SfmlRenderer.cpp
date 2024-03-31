#include "SfmlRenderer.h"

SfmlRenderer::SfmlRenderer(sf::RenderWindow* window) {
    this->window = window;

    this->scale = 15; // Scale up because 64 x 32 would be tiny on our screens now
}

void SfmlRenderer::drawPixel(uint16_t x, uint16_t y) {
    sf::RectangleShape rectangle(sf::Vector2f(getScale(), getScale()));
    rectangle.setPosition(x * getScale(), y * getScale());
    rectangle.setFillColor(sf::Color::White);

    this->window->draw(rectangle);
}

uint8_t SfmlRenderer::getScale() const {
    return this->scale;
}