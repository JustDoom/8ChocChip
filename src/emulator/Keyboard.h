#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <cstdint>
#include <functional>
#include <unordered_map>

class Keyboard {
public:
    std::unordered_map<uint8_t, bool> keysPressed;
    std::function<void(unsigned char)> onNextKeyPress;

    void setOnNextKeyPress(std::function<void(unsigned char)> callback);
    void handleKeyDown(uint8_t keyCode);
    void handleKeyUp(uint8_t keyCode);
    bool isKeyPressed(int keyCode);
};



#endif //KEYBOARD_H
