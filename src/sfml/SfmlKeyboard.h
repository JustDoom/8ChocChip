#ifndef INC_8CHOCCHIP_SFMLKEYBOARD_H
#define INC_8CHOCCHIP_SFMLKEYBOARD_H

#include "../emulator/Keyboard.h"

class SfmlKeyboard : public Keyboard {
public:
    void handleKeyDown(uint8_t keyCode) override;
    void handleKeyUp(uint8_t keyCode) override;
};

#endif // INC_8CHOCCHIP_SFMLKEYBOARD_H
