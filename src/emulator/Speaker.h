//
// Created by doom on 19/03/24.
//

#ifndef SPEAKER_H
#define SPEAKER_H

#include <SFML/Audio.hpp>

class Speaker {
public:
    Speaker();

    void play();
    void stop();
private:
    sf::SoundBuffer soundBuffer;
    sf::Sound sound;
};



#endif //SPEAKER_H
