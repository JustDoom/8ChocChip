#ifndef CPU_H
#define CPU_H

#include <array>
#include <cstdint>
#include <fstream>

#include "Keyboard.h"
#include "Renderer.h"
#include "Speaker.h"

#include "../Settings.h"

class Cpu {
private:
    std::array<uint8_t, 4096> memory;
    std::array<uint8_t, 16> registers;
    std::array<uint16_t, 16> stack;
    uint16_t address;
    uint16_t pc;
    uint8_t sp;
    uint8_t delay;
    uint8_t soundTimer;

    bool drawn;
    uint32_t speed;
    uint8_t seed;

    Renderer* renderer;
    Keyboard* keyboard;
    Speaker* speaker;

    const RomSettings romSettings;
public:
    Cpu(Renderer* renderer, Keyboard* keyboard, Speaker* speaker, RomSettings romSettings);

    void loadSpritesIntoMemory();
    void loadProgramIntoMemory(std::ifstream* file);

    void cycle();
    void runInstruction();

    uint8_t random8bit();

    uint64_t instructions = 0;
};

#endif