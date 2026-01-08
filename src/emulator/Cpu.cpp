#include "Cpu.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <random>

Cpu::Cpu(Keyboard* keyboard, Speaker* speaker, const RomSettings romSettings, std::unordered_map<uint8_t, unsigned char> keymap) : keyboard(keyboard), speaker(speaker), romSettings(romSettings) {
    this->address = 0;
    this->delay = 0;
    this->soundTimer = 0;
    this->pc = 0x200;
    this->sp = 0;
    this->drawn = false;
    this->speed = ipf;

    this->display.fill(0ULL);
    this->memory.fill(0);
    this->registers.fill(0);
    this->stack.fill(0);

    this->keyboard->keymap = keymap;
}

void Cpu::loadSpritesIntoMemory() {
    std::memcpy(&memory[0x50], this->SPRITES.data(), this->SPRITES.size());
}

void Cpu::loadProgramIntoMemory(std::ifstream* file) {
    if (file->is_open()) {
        const std::streampos size = file->tellg();
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
    uint64_t count = 0;
    if (speedTest) {
        for (int i = 0; i < this->speed; i++) {
            runInstruction();
            count++;
        }
    } else {
        for (int i = 0; i < this->speed && !this->drawn; i++) {
            runInstruction();
            count++;
        }
    }

    this->instructions += count;
    this->drawn = false;

    if (this->delay > 0) {
        this->delay--;
    }

    if (this->soundTimer > 0) {
        this->soundTimer--;
    }

    // Play sound until timer runs out
    if (this->soundTimer > 0) {
        this->speaker->play();
    } else {
        this->speaker->stop();
    }
}

void Cpu::runInstruction() {
    const uint8_t second = this->memory[this->pc + 1];
    const uint16_t opcode = (this->memory[this->pc] << 8) | second;
    this->pc += 2;

    const uint8_t x = (opcode >> 8) & 0xF;
    const uint8_t y = (opcode >> 4) & 0xF;

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode) {
                // Clear screen
                case 0x00E0:
                    this->display.fill(0ULL);
                    break;
                case 0x00EE:
                    this->pc = this->stack[--this->sp & 0xF];
                    break;
                default:
                    throw opcode;
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
            if (this->registers[x] == second) {
                this->pc += 2;
            }
            break;
        case 0x4000:
            if (this->registers[x] != second) {
                this->pc += 2;
            }
            break;
        case 0x5000:
            if (this->registers[x] == this->registers[y]) {
                this->pc += 2;
            }
            break;
        case 0x6000:
            this->registers[x] = second;
            break;
        case 0x7000:
            this->registers[x] += second;
            break;
        case 0x8000:
            switch (opcode & 0xF) {
                case 0x0:
                    this->registers[x] = this->registers[y];
                    break;
                case 0x1: {
                    this->registers[x] |= this->registers[y];
                    if (this->romSettings.quirks.logic) {
                        this->registers[0xF] = 0;
                    }
                    break;
                }
                case 0x2: {
                    this->registers[x] &= this->registers[y];
                    if (this->romSettings.quirks.logic) {
                        this->registers[0xF] = 0;
                    }
                    break;
                }
                case 0x3: {
                    this->registers[x] ^= this->registers[y];
                    if (this->romSettings.quirks.logic) {
                        this->registers[0xF] = 0;
                    }
                    break;
                }
                case 0x4: {
                    const uint16_t sum = this->registers[x] + this->registers[y];
                    this->registers[x] = sum & 0xFF;
                    this->registers[0xF] = sum > 0xFF ? 1 : 0;
                    break;
                }
                case 0x5: {
                    const uint8_t value = this->registers[x] >= this->registers[y] ? 1 : 0;
                    this->registers[x] -= this->registers[y];
                    this->registers[0xF] = value;
                    break;
                }
                case 0x6: {
                    if (!this->romSettings.quirks.shift) {
                        this->registers[x] = this->registers[y];
                    }
                    const uint8_t value = (this->registers[x] & 0x1);
                    this->registers[x] >>= 1;
                    this->registers[0xF] = value;
                    break;
                }
                case 0x7: {
                    const uint8_t value =  this->registers[y] >= this->registers[x] ? 1 : 0;
                    this->registers[x] = this->registers[y] - this->registers[x];
                    this->registers[0xF] = value;
                    break;
                }
                case 0xE: {
                    if (!this->romSettings.quirks.shift) {
                        this->registers[x] = this->registers[y];
                    }
                    const uint8_t value = (this->registers[x] & 0x80) >> 7;
                    this->registers[x] <<= 1;
                    this->registers[0xF] = value;
                    break;
                }
                default:
                    throw opcode;
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
            if (this->romSettings.quirks.jump) {
                this->pc = (opcode & 0xFFF) + this->registers[x];
            } else {
                this->pc = (opcode & 0xFFF) + this->registers[0];
            }
            break;
        case 0xC000: {
            this->registers[x] = (random8bit() & 0xFF) & second;
            break;
        }
        case 0xD000: {
                const uint8_t height = opcode & 0xF;
                const uint8_t uX = this->registers[x] & 63;
                const uint8_t uY = this->registers[y] & 31;
                const uint8_t* sprites = &this->memory[this->address];
                uint8_t& vF = this->registers[0xF]; // Store reference to not fetching it every time
                vF = 0;

                for (uint8_t row = 0; row < height; ++row) {
                    uint8_t sprite = sprites[row];
                    if (sprite == 0) {
                        continue;
                    }
                    const uint8_t drawY = uY + row;
                    if (drawY > 31) {
                        continue;
                    }

                    uint64_t& pixelRow = this->display[drawY];
                    uint8_t col = 0;
                    while (sprite) {
                        if (sprite & 0x80) {
                            const uint64_t mask = 1ULL << (63 - (uX + col) & 63);
                            if (pixelRow & mask) {
                                vF = 1;
                            }
                            pixelRow ^= mask;
                        }
                        sprite <<= 1;
                        if (++col == 8) {
                            break;
                        }
                    }
                }
                if (this->romSettings.quirks.vblank) {
                    this->drawn = true;
                }
                break;
        }
        case 0xE000:
            switch (second) {
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
                    throw opcode;
            }

            break;
        case 0xF000:
            switch (second) {
                case 0x07:
                    this->registers[x] = this->delay;
                    break;
                case 0x0A:
                    this->pc -= 2;
                    if (this->keyboard->onNextKeyPress != nullptr) {
                        break;
                    }
                    this->keyboard->setOnNextKeyPress([this, x](const uint8_t key) {
                        this->registers[x] = key;
                        this->pc += 2;
                    });
                    break;
                case 0x15:
                    this->delay = this->registers[x];
                    break;
                case 0x18:
                    this->soundTimer = this->registers[x];
                    break;
                case 0x1E:
                    this->address = this->address + this->registers[x] & 0xFFF;
                    break;
                case 0x29:
                    this->address = 0x50 + (this->registers[x] & 0xF) * 5;
                    break;
                case 0x33: {
                    uint8_t value = this->registers[x];

                    this->memory[this->address + 2 & 0xFFF] = value % 10;
                    value /= 10;
                    this->memory[this->address + 1 & 0xFFF] = value % 10;
                    value /= 10;
                    this->memory[this->address] = value % 10;

                    break;
                }
                case 0x55: {
                    std::memcpy(&memory[address & 0xFFF], &registers[0], x + 1);
                    if (!this->romSettings.quirks.memoryLeaveIUnchanged) {
                        this->address = this->address + (this->romSettings.quirks.memoryIncrementByX ? x : x + 1) & 0xFFF;
                    }
                    break;
                }
                case 0x65: {
                    std::memcpy(&registers[0], &memory[address & 0xFFF], x + 1);
                    if (!this->romSettings.quirks.memoryLeaveIUnchanged) {
                        this->address = this->address + (this->romSettings.quirks.memoryIncrementByX ? x : x + 1) & 0xFFF;
                    }
                    break;
                }
                default:
                    throw opcode;
            }
            break;

        default:
            throw opcode;
    }
}

uint8_t Cpu::random8bit() {
    this->seed ^= static_cast<uint8_t>(time(nullptr) & 0xFF);
    this->seed = (this->seed * 1103515245 + 12345) & 0xFF;
    return this->seed;
}

std::array<uint64_t, 32>& Cpu::getDisplay() {
    return this->display;
}

std::vector<uint8_t> Cpu::serialize() const {
    std::vector<uint8_t> serializedData;
    serializedData.reserve(serializationDimension);

    serializedData.insert(serializedData.end(), memory.cbegin(), memory.cend());
    serializedData.insert(serializedData.end(), registers.cbegin(), registers.cend());
    
    for (int i = 0; i < this->stack.size(); i++) {
        serializedData.push_back(this->stack[i] >> 8);
        serializedData.push_back(this->stack[i] & 0xFF);
    }

    serializedData.push_back(this->address >> 8);
    serializedData.push_back(this->address & 0xFF);

    serializedData.push_back(this->pc >> 8);
    serializedData.push_back(this->pc & 0xFF);

    serializedData.push_back(this->sp);
    serializedData.push_back(this->delay);
    serializedData.push_back(this->soundTimer);
    
    serializedData.push_back(this->drawn);
    serializedData.push_back(this->speed >> 24);
    serializedData.push_back(this->speed >> 16);
    serializedData.push_back(this->speed >> 8);
    serializedData.push_back(this->speed & 0xFF);
    serializedData.push_back(this->seed);

    serializedData.push_back(static_cast<uint8_t>((this->instructions >> 56) & 0xFF));
    serializedData.push_back(static_cast<uint8_t>((this->instructions >> 48) & 0xFF));
    serializedData.push_back(static_cast<uint8_t>((this->instructions >> 32) & 0xFF));
    serializedData.push_back(static_cast<uint8_t>((this->instructions >> 24) & 0xFF));
    serializedData.push_back(static_cast<uint8_t>((this->instructions >> 16) & 0xFF));
    serializedData.push_back(static_cast<uint8_t>((this->instructions >> 8) & 0xFF));
    serializedData.push_back(static_cast<uint8_t>((this->instructions & 0xFF)));

    serializedData.insert(serializedData.end(), this->display.cbegin(), this->display.cend());

    return serializedData;
}

void Cpu::deserialize(uint8_t* serialization) {
    int currentPosition = 0;

    std::copy(serialization + currentPosition,
         serialization + currentPosition + this->memory.size(),
         this->memory.begin());
    currentPosition += this->memory.size();


    std::copy(serialization + currentPosition,
         serialization + currentPosition + this->registers.size(),
        this->registers.begin());
    currentPosition += this->registers.size();

    for (int i = 0; i < this->stack.size(); i++) {
        this->stack[i] = serialization[currentPosition] << 8;
        this->stack[i] |= serialization[currentPosition + 1];
        currentPosition += 2;
    }

    this->address = serialization[currentPosition] << 8;
    this->address |= serialization[currentPosition + 1];
    currentPosition += 2;

    this->pc = serialization[currentPosition] << 8;
    this->pc |= serialization[currentPosition + 1];
    currentPosition += 2;

    this->sp = serialization[currentPosition];
    currentPosition++;
    this->delay = serialization[currentPosition];
    currentPosition++;
    this->soundTimer = serialization[currentPosition];
    currentPosition++;
    
    this->drawn = serialization[currentPosition];
    currentPosition++;
    this->speed = serialization[currentPosition] << 24;
    currentPosition++;
    this->speed |= serialization[currentPosition] << 16;
    currentPosition++;
    this->speed |= serialization[currentPosition] << 8;
    currentPosition++;
    this->speed |= serialization[currentPosition];
    currentPosition++;
    
    this->seed = serialization[currentPosition];
    currentPosition++;

    this->instructions = static_cast<uint64_t>(serialization[currentPosition]) << 56;
    currentPosition++;
    this->instructions |= static_cast<uint64_t>(serialization[currentPosition]) << 48;
    currentPosition++;
    this->instructions |= static_cast<uint64_t>(serialization[currentPosition]) << 32;
    currentPosition++;
    this->instructions |= static_cast<uint64_t>(serialization[currentPosition]) << 24;
    currentPosition++;
    this->instructions |= static_cast<uint64_t>(serialization[currentPosition]) << 16;
    currentPosition++;
    this->instructions |= static_cast<uint64_t>(serialization[currentPosition]) << 8;
    currentPosition++;
    this->instructions |= serialization[currentPosition] & 0xFF;
    currentPosition++;

    std::copy(
        serialization + currentPosition,
        serialization + currentPosition + this->display.size(),
        this->display.begin()
    );
}