#ifndef RENDERER_H
#define RENDERER_H

#include <SDL3/SDL.h>
#include <cstdint>
#include <vector>

class Renderer {
public:
    Renderer();

    bool setPixel(uint8_t x, uint16_t y);

    void clear();

    void render(SDL_Renderer* renderer);

    uint16_t getColumns() const;
    uint16_t getRows() const;

    virtual void drawPixel(SDL_Renderer* renderer, uint16_t x, uint16_t y) = 0;
private:
    const uint16_t columns = 64;
    const uint16_t rows = 32;

    std::vector<bool> display;
};

#endif //RENDERER_H
