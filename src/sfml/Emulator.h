#ifndef INC_8CHOCCHIP_EMULATOR_H
#define INC_8CHOCCHIP_EMULATOR_H

#include <string>

#include "SFML/Graphics.hpp"

class Emulator {
private:
public:
    int launch(const std::string& rom);
};


#endif // INC_8CHOCCHIP_EMULATOR_H
