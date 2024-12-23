#include "Cpu.h"

#include <iostream>
#include <random>

Cpu::Cpu(Renderer* renderer, Keyboard* keyboard, Speaker * speaker) {
    this->address = 0;
    this->delay = 0;
    this->soundTimer = 0;
    this->pc = 0x200;
    this->drawn = false;
    this->paused = false;
    this->speed = 15;

    this->renderer = renderer;
    this->keyboard = keyboard;
    this->speaker = speaker;
}

void Cpu::loadSpritesIntoMemory() {
    const std::vector<uint8_t> sprites = {
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

    for (unsigned int i = 0; i < sprites.size(); ++i) {
        this->memory[0x50 + i] = sprites[i];
    }
}

void Cpu::loadProgramIntoMemory(std::ifstream* file) {
    if (file->is_open()) {
        std::streampos size = file->tellg();
        std::vector<char> buffer(size);
        file->seekg(0, std::ios::beg);
        file->read(buffer.data(), size);
        file->close();

        // Load ROM into memory
        for (size_t loc = 0; loc < buffer.size(); ++loc) {
            this->memory[0x200 + loc] = buffer[loc];
        }
    }
}

void Cpu::cycle() {
    for (int i = 0; i < this->speed; i++) {
        if (this->paused || this->drawn) {
            break;
        }

        runInstruction((this->memory[this->pc] << 8) | this->memory[this->pc + 1]);
    }
    this->drawn = false;

    if (this->paused && this->keyboard->onNextKeyPress == nullptr) {
        this->paused = false;
    }

    if (this->delay > 0) {
        this->delay -= 1;
    }

    if (!this->paused && this->soundTimer > 0) {
        this->soundTimer -= 1;
    }

    // Play sound until timer runs out
    if (this->soundTimer > 0) {
        this->speaker->play();
    } else {
        this->speaker->stop();
    }
}

void Cpu::runInstruction(const uint16_t opcode) {
    this->pc += 2;

    const uint8_t x = (opcode & 0x0F00) >> 8;
    const uint8_t y = (opcode & 0x00F0) >> 4;

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode) {
                // Clear screen
                case 0x00E0:
                    this->renderer->clear();
                    break;
                case 0x00EE:
                    this->pc = this->stack[--this->sp & 0xF];
                    break;
                default:
                    break;
            }

            break;
        case 0x1000:
            this->pc = (opcode & 0xFFF);
            break;
        case 0x2000:
            this->stack[this->sp++ & 0xF] = this->pc;
            this->pc = (opcode & 0xFFF);
            break;
        case 0x3000:
            if (this->registers[x] == (opcode & 0xFF)) {
                this->pc += 2;
            }
            break;
        case 0x4000:
            if (this->registers[x] != (opcode & 0xFF)) {
                this->pc += 2;
            }
            break;
        case 0x5000:
            if (this->registers[x] == this->registers[y]) {
                this->pc += 2;
            }
            break;
        case 0x6000:
            this->registers[x] = (opcode & 0xFF);
            break;
        case 0x7000:
            this->registers[x] += (opcode & 0xFF);
            break;
        case 0x8000:
            switch (opcode & 0xF) {
                case 0x0:
                    this->registers[x] = this->registers[y];
                    break;
                case 0x1: {
                    const uint8_t vY = this->registers[y];
                    this->registers[0xF] = 0;
                    this->registers[x] |= vY;
                    break;
                }
                case 0x2: {
                    const uint8_t vY = this->registers[y];
                    this->registers[0xF] = 0;
                    this->registers[x] &= vY;
                    break;
                }
                case 0x3: {
                    const uint8_t vY = this->registers[y];
                    this->registers[0xF] = 0;
                    this->registers[x] ^= vY;
                    break;
                }
                case 0x4: {
                    const uint16_t sum = this->registers[x] + this->registers[y];

                    this->registers[x] = sum & 0xFF;

                    if (sum > 0xFF) {
                        this->registers[0xF] = 1;
                    } else {
                        this->registers[0xF] = 0;
                    }
                    break;
                }
                case 0x5: {
                    const uint8_t value = this->registers[x] >= this->registers[y] ? 1 : 0;

                    this->registers[x] -= this->registers[y];
                    this->registers[0xF] = value;
                    break;
                }
                case 0x6: {
                    this->registers[x] = this->registers[y]; // TODO: Optional quirk? https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#8xy6-and-8xye-shift
                    const uint8_t value = (this->registers[x] & 0x1);
                    this->registers[x] >>= 1;
                    this->registers[0xF] = value;
                    break;
                }
                case 0x7:

                    this->registers[x] = this->registers[y] - this->registers[x];

                    if (this->registers[y] >= this->registers[x]) {
                        this->registers[0xF] = 1;
                    } else {
                        this->registers[0xF] = 0;
                    }

                    break;
                case 0xE: {
                    this->registers[x] = this->registers[y];
                    const uint8_t value = (this->registers[x] & 0x80) >> 7;
                    this->registers[x] <<= 1;
                    this->registers[0xF] = value;
                    break;
                }
                default:
                    break;
            }

            break;
        case 0x9000:
            if (this->registers[x] != this->registers[y]) {
                this->pc += 2;
            }
            break;
        case 0xA000:
            this->address = (opcode & 0xFFF);
            break;
        case 0xB000:
            this->pc = (opcode & 0xFFF) + this->registers[0];
            break;
        case 0xC000: {
            this->registers[x] = (random8bit() & 0xFF) & (opcode & 0xFF);
            break;
        }
        case 0xD000: {
            const uint8_t height = (opcode & 0xF);

            const uint8_t uX = this->registers[x] % 64;
            const uint8_t uY = this->registers[y] % 32;
            this->registers[0xF] = 0;

            for (uint8_t row = 0; row < height; ++row) {
                uint8_t sprite = this->memory[this->address + row];

                for (uint8_t col = 0; col < 8; ++col) {
                    if (sprite & 0x80) {
                        const uint8_t drawX = (uX + col);
                        const uint8_t drawY = (uY + row);

                        if (drawX >= 64 || drawY >= 32) continue;

                        if (this->renderer->setPixel(drawX, drawY)) {
                            this->registers[0xF] = 1;
                        }
                    }
                    sprite <<= 1;
                }
            }
            this->drawn = true;
            break;
        }
        case 0xE000:
            switch (opcode & 0xFF) {
                case 0x9E:
                    if (this->keyboard->isKeyPressed(this->registers[x] & 0xF)) {
                        this->pc += 2;
                    }
                    break;
                case 0xA1:
                    if (!this->keyboard->isKeyPressed(this->registers[x] & 0xF)) {
                        this->pc += 2;
                    }
                    break;
                default:
                    break;
            }

            break;
        case 0xF000:
            switch (opcode & 0xFF) {
                case 0x07:
                    this->registers[x] = this->delay;
                    break;
                case 0x0A:
                    this->paused = true;
                    this->keyboard->onNextKeyPress = [&](const uint8_t key) {
                        this->registers[x] = key;
                        this->paused = false;
                    };
                    break;
                case 0x15:
                    this->delay = this->registers[x];
                    break;
                case 0x18:
                    this->soundTimer = this->registers[x];
                    break;
                case 0x1E:
                    this->address += this->registers[x];
                    break;
                case 0x29:
                    this->address = 0x50 + (this->registers[x] & 0xF) * 5;
                    break;
                case 0x33: {
                    uint8_t value = this->registers[x];

                    this->memory[this->address + 2] = value % 10;
                    value /= 10;

                    this->memory[this->address + 1] = (value % 10);
                    value /= 10;

                    this->memory[this->address] = value % 10;
                    break;
                }
                case 0x55: {
                    for (uint8_t registerIndex = 0; registerIndex <= x; ++registerIndex) {
                        this->memory[this->address + registerIndex] = this->registers[registerIndex];
                    }
                    this->address += x + 1;
                    break;
                }
                case 0x65: {
                    for (uint8_t registerIndex = 0; registerIndex <= x; ++registerIndex) {
                        this->registers[registerIndex] = this->memory[this->address + registerIndex];
                    }
                    this->address += x + 1;
                    break;
                }
                default:
                    break;
            }

            break;

        default:
            std::cerr << "Unknown code" << opcode << std::endl;
            break;
    }
}

uint8_t Cpu::random8bit() {
    this->seed ^= static_cast<uint8_t>(time(nullptr) & 0xFF);
    this->seed = (this->seed * 1103515245 + 12345) & 0xFF;
    return this->seed;
}