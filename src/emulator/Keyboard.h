#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <cstdint>
#include <functional>
#include <unordered_map>

class Keyboard {
public:
    std::unordered_map<uint8_t, bool> keysPressed;
    std::function<void(unsigned char)> onNextKeyPress;
    std::unordered_map<uint8_t, unsigned char> KEYMAP = {
        {27, 0x1}, // 1
        {28, 0x2}, // 2
        {29, 0x3}, // 3
        {30, 0xc}, // 4
        {16, 0x4}, // Q
        {22, 0x5}, // W
        {4, 0x6}, // E
        {17, 0xD}, // R
        {0, 0x7}, // A
        {18, 0x8}, // S
        {3, 0x9}, // D
        {5, 0xE}, // F
        {25, 0xA}, // Z
        {23, 0x0}, // X
        {2, 0xB}, // C
        {21, 0xF}  // V
    };

    void setOnNextKeyPress(std::function<void(unsigned char)> callback);
    void handleKeyDown(uint8_t keyCode);
    void handleKeyUp(uint8_t keyCode);
    bool isKeyPressed(int keyCode);
};



#endif //KEYBOARD_H
