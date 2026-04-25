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

    this->seed = static_cast<uint8_t>(time(nullptr) & 0xFF);
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
    runInstruction();

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
    static const void* const table[16] = {
        &&op0, &&op1, &&op2, &&op3, &&op4, &&op5, &&op6, &&op7,
        &&op8, &&op9, &&opA, &&opB, &&opC, &&opD, &&opE, &&opF
    };

    uint8_t* const mem = this->memory.data();
    uint8_t* const regs = this->registers.data();
    uint64_t* const disp = this->display.data();
    uint16_t* const stk = this->stack.data();

    const uint32_t budget = this->speed;
    const bool stopOnDrawn = !speedTest;
    uint32_t executed = 0;
    uint16_t pc = this->pc;
    uint16_t opcode = 0;
    uint8_t x = 0, y = 0, second = 0;
    uint16_t nnn = 0;

    #define FETCH_DISPATCH()                                       \
        do {                                                       \
            if (executed >= budget) goto threadedEnd;              \
            if (stopOnDrawn && this->drawn) goto threadedEnd;      \
            ++executed;                                            \
            uint16_t raw__;                                        \
            std::memcpy(&raw__, mem + pc, sizeof(raw__));          \
            opcode = __builtin_bswap16(raw__);                     \
            pc += 2;                                               \
            x = (opcode >> 8) & 0xF;                               \
            y = (opcode >> 4) & 0xF;                               \
            second = opcode & 0xFF;                                \
            nnn = opcode & 0xFFF;                                  \
            goto *table[opcode >> 12];                             \
        } while (0)

    #define THROW_OPCODE()                                         \
        do {                                                       \
            this->pc = pc;                                         \
            this->instructions += executed;                        \
            throw opcode;                                          \
        } while (0)

    FETCH_DISPATCH();

op0:
    switch (opcode) {
        case 0x00E0: instruction00E0(); break;
        case 0x00EE: instruction00EE(pc, stk); break;
        default: THROW_OPCODE();
    }
    FETCH_DISPATCH();

op1:
    instruction1nnn(pc, nnn);
    FETCH_DISPATCH();

op2:
    instruction2nnn(pc, nnn, stk);
    FETCH_DISPATCH();

op3:
    instruction3xnn(pc, second, x);
    FETCH_DISPATCH();

op4:
    instruction4xnn(pc, second, x);
    FETCH_DISPATCH();

op5:
    instruction5xy0(pc, x, y);
    FETCH_DISPATCH();

op6:
    instruction6xnn(second, x);
    FETCH_DISPATCH();

op7:
    instruction7xnn(second, x);
    FETCH_DISPATCH();

op8:
    instruction8xy(opcode, x, y);
    FETCH_DISPATCH();

op9:
    instruction9xy0(pc, x, y);
    FETCH_DISPATCH();

opA:
    instructionAnnn(nnn);
    FETCH_DISPATCH();

opB:
    instructionBnnn(nnn, x);
    FETCH_DISPATCH();

opC:
    instructionCxnn(second, x);
    FETCH_DISPATCH();

opD:
    instructionDxyn(opcode, x, y);
    FETCH_DISPATCH();

opE:
    switch (second) {
        case 0x9E:
            instructionEx9e(pc, x);
            break;
        case 0xA1:
            instructionExA1(pc, x);
            break;
        default: THROW_OPCODE();
    }
    FETCH_DISPATCH();

opF:
    instructionFx(pc, second, x);
    FETCH_DISPATCH();

threadedEnd:
    this->pc = pc;
    this->instructions += executed;

    #undef FETCH_DISPATCH
    #undef THROW_OPCODE
}

void Cpu::instruction00E0() {
    std::memset(this->display.data(), 0, 32 * sizeof(uint64_t));
}

void Cpu::instruction00EE(uint16_t& pc, const uint16_t* stk) {
    pc = stk[--this->sp & 0xF];
}

void Cpu::instruction1nnn(uint16_t& pc, const uint16_t nnn) {
    pc = nnn;
}

void Cpu::instruction2nnn(uint16_t& pc, const uint16_t nnn, uint16_t* stk) {
    stk[this->sp++ & 0xF] = pc;
    pc = nnn;
}

void Cpu::instruction3xnn(uint16_t& pc, const uint8_t second, const uint8_t x) const {
    if (this->registers[x] == second) pc += 2;
}

void Cpu::instruction4xnn(uint16_t& pc, const uint8_t second, const uint8_t x) const {
    if (this->registers[x] != second) pc += 2;
}

void Cpu::instruction5xy0(uint16_t& pc, const uint8_t x, const uint8_t y) const {
    if (this->registers[x] == this->registers[y]) pc += 2;
}

void Cpu::instruction6xnn(const uint8_t second, const uint8_t x) {
    this->registers[x] = second;
}

void Cpu::instruction7xnn(const uint8_t second, const uint8_t x) {
    this->registers[x] += second;
}

void Cpu::instruction8xy(const uint16_t opcode, const uint8_t x, const uint8_t y) {
    switch (opcode & 0xF) {
        case 0x0: this->registers[x] = this->registers[y]; break;
        case 0x1:
            this->registers[x] |= this->registers[y];
            if (this->romSettings.quirks.logic) this->registers[0xF] = 0;
            break;
        case 0x2:
            this->registers[x] &= this->registers[y];
            if (this->romSettings.quirks.logic) this->registers[0xF] = 0;
            break;
        case 0x3:
            this->registers[x] ^= this->registers[y];
            if (this->romSettings.quirks.logic) this->registers[0xF] = 0;
            break;
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
            if (!this->romSettings.quirks.shift) this->registers[x] = this->registers[y];
            const uint8_t value = this->registers[x] & 0x1;
            this->registers[x] >>= 1;
            this->registers[0xF] = value;
            break;
        }
        case 0x7: {
            const uint8_t value = this->registers[y] >= this->registers[x] ? 1 : 0;
            this->registers[x] = this->registers[y] - this->registers[x];
            this->registers[0xF] = value;
            break;
        }
        case 0xE: {
            if (!this->romSettings.quirks.shift) this->registers[x] = this->registers[y];
            const uint8_t value = (this->registers[x] & 0x80) >> 7;
            this->registers[x] <<= 1;
            this->registers[0xF] = value;
            break;
        }
        // default: THROW_OPCODE();
    }
}

void Cpu::instruction9xy0(uint16_t& pc, const uint8_t x, const uint8_t y) const {
    if (this->registers[x] != this->registers[y]) pc += 2;
}

void Cpu::instructionAnnn(const uint16_t nnn) {
    this->address = nnn;
}

void Cpu::instructionBnnn(const uint16_t nnn, const uint8_t x) {
    if (this->romSettings.quirks.jump) pc = nnn + this->registers[x];
    else pc = nnn + this->registers[0];
}

void Cpu::instructionCxnn(const uint8_t second, const uint8_t x) {
    this->registers[x] = (random8bit() & 0xFF) & second;
}

void Cpu::instructionDxyn(const uint16_t opcode, const uint8_t x, const uint8_t y) {
    const uint8_t height = opcode & 0xF;
    const uint8_t uX = this->registers[x] & 63;
    const uint8_t uY = this->registers[y] & 31;
    const uint8_t* sprites = &this->memory[this->address];
    uint8_t& vF = this->registers[0xF];
    vF = 0;
    const int shift = 56 - static_cast<int>(uX);
    for (uint8_t row = 0; row < height; ++row) {
        const uint8_t sprite = sprites[row];
        if (sprite == 0) continue;
        const uint8_t drawY = uY + row;
        if (drawY > 31) continue;
        uint64_t spritePlaced;
        if (shift >= 0) spritePlaced = static_cast<uint64_t>(sprite) << shift;
        else spritePlaced = static_cast<uint64_t>(sprite) >> (-shift);
        uint64_t& pixelRow = this->display[drawY];
        if (pixelRow & spritePlaced) vF = 1;
        pixelRow ^= spritePlaced;
    }
    if (this->romSettings.quirks.vblank) this->drawn = true;
}

void Cpu::instructionEx9e(uint16_t& pc, const uint8_t x) const {
    if (this->keyboard->isKeyPressed(this->registers[x] & 0xF)) pc += 2;
}

void Cpu::instructionExA1(uint16_t& pc, const uint8_t x) const {
    if (!this->keyboard->isKeyPressed(this->registers[x] & 0xF)) pc += 2;
}

void Cpu::instructionFx(uint16_t& pc, const uint8_t second, const uint8_t x) {
    switch (second) {
        case 0x07: this->registers[x] = this->delay; break;
        case 0x0A:
            pc -= 2;
            if (this->keyboard->onNextKeyPress != nullptr) break;
            this->pc = pc;
            this->keyboard->setOnNextKeyPress([this, x](const uint8_t key) {
                this->registers[x] = key;
                this->pc += 2;
            });
            break;
        case 0x15: this->delay = this->registers[x]; break;
        case 0x18: this->soundTimer = this->registers[x]; break;
        case 0x1E: this->address = (this->address + this->registers[x]) & 0xFFF; break;
        case 0x29: this->address = 0x50 + (this->registers[x] & 0xF) * 5; break;
        case 0x33: {
            uint8_t value = this->registers[x];
            this->memory[(this->address + 2) & 0xFFF] = value % 10;
            value /= 10;
            this->memory[(this->address + 1) & 0xFFF] = value % 10;
            value /= 10;
            this->memory[this->address] = value % 10;
            break;
        }
        case 0x55:
            std::memcpy(&this->memory[this->address & 0xFFF], this->registers.data(), x + 1);
            if (!this->romSettings.quirks.memoryLeaveIUnchanged) {
                this->address = (this->address + (this->romSettings.quirks.memoryIncrementByX ? x : x + 1)) & 0xFFF;
            }
            break;
        case 0x65:
            std::memcpy(this->registers.data(), &this->memory[this->address & 0xFFF], x + 1);
            if (!this->romSettings.quirks.memoryLeaveIUnchanged) {
                this->address = (this->address + (this->romSettings.quirks.memoryIncrementByX ? x : x + 1)) & 0xFFF;
            }
            break;
        // default: THROW_OPCODE();
    }
}

uint8_t Cpu::random8bit() {
    return (this->seed * 1103515245 + 12345) & 0xFF;
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