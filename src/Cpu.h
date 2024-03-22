//
// Created by doom on 19/03/24.
//

#ifndef CPU_H
#define CPU_H

#include <vector>
#include <random>
#include <fstream>
#include <iostream>
#include "Keyboard.h"
#include "Speaker.h"
#include "Renderer.h"

class Cpu {
public:
    Cpu(Renderer* renderer, Keyboard* keyboard, Speaker* speaker);

    void loadSpritesIntoMemory();
    void loadProgramIntoMemory(const char *filename);

    void cycle();
    void runInstruction(u_int16_t opcode);
    void updateTimers();
private:
    std::vector<u_int8_t> memory;
    std::vector<u_int8_t> registers;
    u_int16_t address;
    u_int8_t delay;
    u_int8_t soundTimer;

    u_int16_t pc;
    std::vector<u_int16_t> stack;
    bool paused;
    int8_t speed;

    Renderer* renderer;
    Keyboard* keyboard;
    Speaker* speaker;
};

#endif //CPU_H
