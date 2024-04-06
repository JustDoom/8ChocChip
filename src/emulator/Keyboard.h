#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <unordered_map>
#include <vector>
#include <functional>
#include <cstdint>

class Keyboard {
public:
    std::unordered_map<uint8_t, bool> keysPressed;
    std::function<void(unsigned char)> onNextKeyPress;

    void setOnNextKeyPress(std::function<void(unsigned char)> callback);
    virtual void handleKeyDown(uint8_t keyCode) = 0;
    virtual void handleKeyUp(uint8_t keyCode) = 0;
    bool isKeyPressed(int keyCode);
};



#endif //KEYBOARD_H
