//
// Created by doom on 19/03/24.
//

#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdint>

class Renderer {
public:
    Renderer();

    bool setPixel(uint8_t x, uint16_t y);

    void clear();

    void render(sf::RenderWindow* window);

    uint16_t getColumns() const;
    uint16_t getRows() const;
    uint8_t getScale() const;
private:
    std::vector<bool> display;
    uint8_t scale;
    uint16_t columns = 64;
    uint16_t rows = 32;

    void drawRectangle(sf::RenderWindow& window, int x, int y, int width, int height);
};

#endif //RENDERER_H
