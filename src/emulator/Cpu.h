#ifndef CPU_H
#define CPU_H

#include <array>

#include "Keyboard.h"
#include "Renderer.h"
#include "Speaker.h"

#include "../Settings.h"

class Cpu {
private:
    alignas(64) std::array<uint8_t, 4096> memory;
    alignas(16) std::array<uint8_t, 16> registers;
    alignas(32) std::array<uint16_t, 16> stack;
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

    static constexpr std::array<uint8_t, 80> SPRITES = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
public:
    Cpu(Renderer* renderer, Keyboard* keyboard, Speaker* speaker, RomSettings romSettings, std::unordered_map<uint8_t, unsigned char> keymap);

    void loadSpritesIntoMemory();
    void loadProgramIntoMemory(std::ifstream* file);

    void cycle();
    void runInstruction();

    uint8_t random8bit();

    std::vector<uint8_t> serialize();
    void deserialize(uint8_t* serialization);
    static const size_t serializationDimension = 4096 + 16 + 32 + 2 + 2 + 1 + 1 + 1 + 1 + 4 + 1 + 8 + 2048;

    uint64_t instructions = 0;
};

#endif