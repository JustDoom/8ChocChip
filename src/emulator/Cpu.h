#ifndef CPU_H
#define CPU_H

#include <array>
#include <cstdint>
#include <fstream>

#include "Keyboard.h"
#include "Renderer.h"
#include "Speaker.h"

class Cpu {
private:
    std::array<uint8_t, 4096> memory;
    std::array<uint8_t, 16> registers;
    uint16_t address{};
    uint16_t pc{};
    uint8_t sp{};
    std::array<uint16_t, 16> stack;
    uint8_t delay;
    uint8_t soundTimer;

    bool drawn;
    bool paused;
    uint32_t speed;
    uint8_t seed;

    Renderer* renderer;
    Keyboard* keyboard;
    Speaker* speaker;
public:
    Cpu(Renderer* renderer, Keyboard* keyboard, Speaker * speaker);

    void loadSpritesIntoMemory();
    void loadProgramIntoMemory(std::ifstream* file);

    void cycle();
    void runInstruction(uint16_t opcode);

    uint8_t random8bit();

    int instructions;
};

#endif