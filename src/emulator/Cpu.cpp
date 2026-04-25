#include "Cpu.h"

#include <cstring>
#include <fstream>
#include <iostream>

Cpu::Cpu(Keyboard* keyboard, Speaker* speaker, const RomSettings romSettings,
         std::unordered_map<uint8_t, unsigned char> keymap) : romSettings(romSettings), keyboard(keyboard),
                                                              speaker(speaker) {
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
    std::memcpy(&memory[0x50], SPRITES.data(), SPRITES.size());
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
    runInstructions();

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

void Cpu::runInstructions() {
    // This method runs as many instructions per frame as needed. Since it could be up to millions (1dcell)
    // Create local variables here for faster and better access to stuff
    static const void* const table[16] = {
        &&op0, &&op1, &&op2, &&op3, &&op4, &&op5, &&op6, &&op7,
        &&op8, &&op9, &&opA, &&opB, &&opC, &&opD, &&opE, &&opF
    };

    uint8_t* const regs = this->registers.data();
    uint8_t* const mem = this->memory.data();
    uint64_t* const disp = this->display.data();
    uint16_t* const stack = this->stack.data();

    // Settings
    const uint32_t budget = this->speed;
    const bool stopOnDrawn = !speedTest;
    const bool quirkLogic = this->romSettings.quirks.logic;
    const bool quirkShift = this->romSettings.quirks.shift;
    const bool quirkJump = this->romSettings.quirks.jump;
    const bool vblank = this->romSettings.quirks.vblank;
    const bool quirkMemLeave = this->romSettings.quirks.memoryLeaveIUnchanged;
    const bool quirkMemIncX = this->romSettings.quirks.memoryIncrementByX;

    uint32_t executed = 0;
    uint16_t pc = this->pc;
    uint16_t address = this->address;
    uint8_t sp = this->sp;
    uint8_t delay = this->delay;
    uint8_t seed = this->seed;
    uint16_t opcode = 0;
    uint8_t x = 0;
    uint8_t second = 0;

    static void* tableF[256];
    static bool tableFInited = false;
    if (__builtin_expect(!tableFInited, 0)) {
        for (auto& e : tableF) {
            e = &&opFErr;
        }
        tableF[0x07] = &&opF07;
        tableF[0x0A] = &&opF0A;
        tableF[0x15] = &&opF15;
        tableF[0x18] = &&opF18;
        tableF[0x1E] = &&opF1E;
        tableF[0x29] = &&opF29;
        tableF[0x33] = &&opF33;
        tableF[0x55] = &&opF55;
        tableF[0x65] = &&opF65;
        tableFInited = true;
    }

#define FETCH_DISPATCH() \
        do { \
            if (executed >= budget) goto threadedEnd; \
            ++executed; \
            uint16_t raw__; \
            std::memcpy(&raw__, mem + pc, sizeof(raw__)); \
            opcode = __builtin_bswap16(raw__); \
            pc += 2; \
            x = (opcode >> 8) & 0xF; \
            second = opcode & 0xFF; \
            goto *table[opcode >> 12]; \
        } while (0)

#define THROW_OPCODE() \
        do { \
            this->pc = pc; \
            this->address = address; \
            this->sp = sp; \
            this->delay = delay; \
            this->seed = seed; \
            this->instructions += executed; \
            throw opcode; \
        } while (0)

    FETCH_DISPATCH();

op0:
    if (opcode == 0x00E0) {
        std::memset(disp, 0, 32 * sizeof(uint64_t));
    } else if (opcode == 0x00EE) {
        pc = stack[--sp & 0xF];
    } else {
        THROW_OPCODE();
    }
    FETCH_DISPATCH();

op1:
    pc = opcode & 0xFFF;
    FETCH_DISPATCH();

op2:
    stack[sp++ & 0xF] = pc;
    pc = opcode & 0xFFF;
    FETCH_DISPATCH();

op3:
    if (regs[x] == second) pc += 2;
    FETCH_DISPATCH();

op4:
    if (regs[x] != second) pc += 2;
    FETCH_DISPATCH();

op5: {
    if (const uint8_t y = second >> 4; regs[x] == regs[y]) pc += 2;
    FETCH_DISPATCH();
}

op6:
    regs[x] = second;
    FETCH_DISPATCH();

op7:
    regs[x] += second;
    FETCH_DISPATCH();

op8: {
    const uint8_t y = second >> 4;
    switch (second & 0xF) {
        case 0x0: regs[x] = regs[y];
            break;
        case 0x1:
            regs[x] |= regs[y];
            if (quirkLogic) regs[0xF] = 0;
            break;
        case 0x2:
            regs[x] &= regs[y];
            if (quirkLogic) regs[0xF] = 0;
            break;
        case 0x3:
            regs[x] ^= regs[y];
            if (quirkLogic) regs[0xF] = 0;
            break;
        case 0x4: {
            const uint16_t sum = static_cast<uint16_t>(regs[x]) + regs[y];
            regs[x] = static_cast<uint8_t>(sum);
            regs[0xF] = sum > 0xFF ? 1 : 0;
            break;
        }
        case 0x5: {
            const uint8_t vf = regs[x] >= regs[y] ? 1 : 0;
            regs[x] -= regs[y];
            regs[0xF] = vf;
            break;
        }
        case 0x6: {
            if (!quirkShift) regs[x] = regs[y];
            const uint8_t vf = regs[x] & 1;
            regs[x] >>= 1;
            regs[0xF] = vf;
            break;
        }
        case 0x7: {
            const uint8_t vf = regs[y] >= regs[x] ? 1 : 0;
            regs[x] = regs[y] - regs[x];
            regs[0xF] = vf;
            break;
        }
        case 0xE: {
            if (!quirkShift) regs[x] = regs[y];
            const uint8_t vf = regs[x] >> 7;
            regs[x] <<= 1;
            regs[0xF] = vf;
            break;
        }
        default: THROW_OPCODE();
        }
        FETCH_DISPATCH();
    }

op9: {
    if (const uint8_t y = second >> 4; regs[x] != regs[y]) pc += 2;
    FETCH_DISPATCH();
}

opA:
    address = opcode & 0xFFF;
    FETCH_DISPATCH();

opB:
    pc = (opcode & 0xFFF) + (quirkJump ? regs[x] : regs[0]);
    FETCH_DISPATCH();

opC:
    seed = static_cast<uint8_t>(seed * 1103515245 + 12345);
    regs[x] = seed & second;
    FETCH_DISPATCH();

opD: {
    const uint8_t height = second & 0xF;
    const uint8_t uX = regs[x] & 63;
    const uint8_t uY = regs[second >> 4] & 31;
    const uint8_t* sprites = &mem[address];
    regs[0xF] = 0;
    const int shift = 56 - static_cast<int>(uX);
    for (uint8_t row = 0; row < height; ++row) {
        const uint8_t sprite = sprites[row];
        if (sprite == 0) continue;
        const uint8_t drawY = uY + row;
        if (drawY > 31) break;
        const uint64_t placed = (shift >= 0) ? (static_cast<uint64_t>(sprite) << shift) : (static_cast<uint64_t>(sprite) >> (-shift));
        if (disp[drawY] & placed) regs[0xF] = 1;
        disp[drawY] ^= placed;
    }
    if (vblank) {
        this->drawn = true;
        if (stopOnDrawn) executed = budget;
    }
    FETCH_DISPATCH();
}

opE:
    if (second == 0x9E) goto opEx9E;
    if (second == 0xA1) goto opExA1;
    THROW_OPCODE();
opEx9E:
    if (this->keyboard->isKeyPressed(regs[x] & 0xF)) pc += 2;
    FETCH_DISPATCH();
opExA1:
    if (!this->keyboard->isKeyPressed(regs[x] & 0xF)) pc += 2;
    FETCH_DISPATCH();

opF:
    goto *tableF[second];
opF07:
    regs[x] = delay;
    FETCH_DISPATCH();
opF0A:
    pc -= 2;
    if (this->keyboard->onNextKeyPress != nullptr) { FETCH_DISPATCH(); }
    this->pc = pc;
    this->keyboard->setOnNextKeyPress([this, x](const uint8_t key) {
        this->registers[x] = key;
        this->pc += 2;
    });
    FETCH_DISPATCH();
opF15:
    delay = regs[x];
    FETCH_DISPATCH();
opF18:
    this->soundTimer = regs[x];
    FETCH_DISPATCH();
opF1E:
    address = (address + regs[x]) & 0xFFF;
    FETCH_DISPATCH();
opF29:
    address = 0x50 + (regs[x] & 0xF) * 5;
    FETCH_DISPATCH();
opF33: {
    uint8_t v = regs[x];
    mem[(address + 2) & 0xFFF] = v % 10;
    v /= 10;
    mem[(address + 1) & 0xFFF] = v % 10;
    v /= 10;
    mem[address & 0xFFF] = v % 10;
    FETCH_DISPATCH();
}
opF55:
    std::memcpy(&mem[address & 0xFFF], regs, x + 1);
    if (!quirkMemLeave)
        address = (address + (quirkMemIncX ? x : x + 1)) & 0xFFF;
    FETCH_DISPATCH();
opF65:
    std::memcpy(regs, &mem[address & 0xFFF], x + 1);
    if (!quirkMemLeave)
        address = (address + (quirkMemIncX ? x : x + 1)) & 0xFFF;
    FETCH_DISPATCH();
opFErr:
    THROW_OPCODE();

threadedEnd:
    this->pc = pc;
    this->address = address;
    this->sp = sp;
    this->delay = delay;
    this->seed = seed;
    this->instructions += executed;

#undef FETCH_DISPATCH
#undef THROW_OPCODE
}

uint8_t Cpu::random8bit() {
    return this->seed = static_cast<uint8_t>(this->seed * 1103515245 + 12345);
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
