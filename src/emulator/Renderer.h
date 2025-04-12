#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

#include <SDL3/SDL.h>

class Renderer {
private:
    const float scale = 15;
    const uint8_t columns = 64;
    const uint8_t rows = 32;
public:
    Renderer() : display(columns * rows) {};
    std::vector<uint8_t> display;

    bool setPixel(uint8_t x, uint8_t y);
    bool getPixel(uint8_t x, uint8_t y);

    void clear();

    void render(SDL_Renderer* renderer);

    uint8_t getColumns() const;
    uint8_t getRows() const;
    float getScale() const;
};

#endif