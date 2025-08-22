#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <cstdint>
#include <functional>
#include <unordered_map>

class Keyboard {
public:
    std::unordered_map<uint8_t, bool> keysPressed;
    std::function<void(unsigned char)> onNextKeyPress;
    std::unordered_map<uint8_t, unsigned char> keymap = {
        {30, 0x1}, // 1
        {31, 0x2}, // 2
        {32, 0x3}, // 3
        {33, 0xc}, // 4
        {20, 0x4}, // Q
        {26, 0x5}, // W
        {8, 0x6}, // E
        {21, 0xD}, // R
        {4, 0x7}, // A
        {22, 0x8}, // S
        {7, 0x9}, // D
        {9, 0xE}, // F
        {29, 0xA}, // Z
        {27, 0x0}, // X
        {6, 0xB}, // C
        {25, 0xF}  // V
    };

    void setOnNextKeyPress(std::function<void(unsigned char)> callback);
    void handleKeyDown(uint8_t keyCode);
    void handleKeyUp(uint8_t keyCode);
    bool isKeyPressed(int keyCode);
};

#endif