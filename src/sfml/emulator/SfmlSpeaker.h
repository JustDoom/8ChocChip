#ifndef INC_8CHOCCHIP_SFMLSPEAKER_H
#define INC_8CHOCCHIP_SFMLSPEAKER_H

#include "SFML/Audio.hpp"

#include "../../emulator/Speaker.h"

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
