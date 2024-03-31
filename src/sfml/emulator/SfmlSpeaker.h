#ifndef INC_8CHOCCHIP_SFMLSPEAKER_H
#define INC_8CHOCCHIP_SFMLSPEAKER_H

#include "../../emulator/Speaker.h"
#include "SFML/Audio.hpp"

class SfmlSpeaker : public Speaker {
private:
    sf::SoundBuffer soundBuffer;
    sf::Sound sound;
public:
    SfmlSpeaker();

    void play() override;
    void stop() override;
};

#endif // INC_8CHOCCHIP_SFMLSPEAKER_H
