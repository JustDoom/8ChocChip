#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <fstream>

#include "Keyboard.h"
#include "Renderer.h"
#include "Speaker.h"

class Cpu {
public:
    Cpu(Renderer* renderer, Keyboard* keyboard, Speaker * speaker);

    void loadSpritesIntoMemory();
    void loadProgramIntoMemory(std::ifstream* file);

    void cycle();
    void runInstruction(uint16_t opcode);
    void updateTimers();
private:
    std::array<uint8_t, 4096> memory;
    std::array<uint8_t, 16> registers;
    uint16_t address{};
    uint16_t pc{};
    uint8_t sp{};
    std::array<uint16_t, 16> stack;
    uint8_t delay;
    uint8_t soundTimer;

    bool paused;
    uint8_t speed;

    Renderer* renderer;
    Keyboard* keyboard;
    Speaker* speaker;
};

#endif //CPU_H
