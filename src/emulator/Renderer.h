#ifndef RENDERER_H
#define RENDERER_H

#include <cstdint>
#include <vector>

#include <SDL3/SDL.h>

class Renderer {
private:
    const float scale = 15;
    const uint16_t columns = 64;
    const uint16_t rows = 32;

    std::vector<bool> display;
public:
    Renderer();

    bool setPixel(uint8_t x, uint16_t y);

    void clear();

    void render(SDL_Renderer* renderer);

    uint16_t getColumns() const;
    uint16_t getRows() const;
    float getScale() const;

    void drawPixel(SDL_Renderer* renderer, uint16_t x, uint16_t y);
};

#endif