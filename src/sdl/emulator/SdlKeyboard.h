#ifndef INC_8CHOCCHIP_SDLKEYBOARD_H
#define INC_8CHOCCHIP_SDLKEYBOARD_H

#include "../../emulator/Keyboard.h"

class SdlKeyboard : public Keyboard {
public:
    void handleKeyDown(uint8_t keyCode);
    void handleKeyUp(uint8_t keyCode);
};

#endif