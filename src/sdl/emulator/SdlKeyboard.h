#ifndef INC_8CHOCCHIP_SDLKEYBOARD_H
#define INC_8CHOCCHIP_SDLKEYBOARD_H

#include "../../emulator/Keyboard.h"
#include <iostream>

class SdlKeyboard : public Keyboard {
private:
    // std::unordered_map<uint8_t, unsigned char> KEYMAP = {
    //         {49, 0x1}, // 1
    //         {50, 0x2}, // 2
    //         {51, 0x3}, // 3
    //         {52, 0xc}, // 4
    //         {113, 0x4}, // Q
    //         {119, 0x5}, // W
    //         {101, 0x6}, // E
    //         {114, 0xD}, // R
    //         {97, 0x7}, // A
    //         {115, 0x8}, // S
    //         {100, 0x9}, // D
    //         {102, 0xE}, // F
    //         {122, 0xA}, // Z
    //         {120, 0x0}, // X
    //         {99, 0xB}, // C
    //         {118, 0xF}  // V
    // };
public:
    void handleKeyDown(uint8_t keyCode);
    void handleKeyUp(uint8_t keyCode);
};

#endif // INC_8CHOCCHIP_SDLKEYBOARD_H
