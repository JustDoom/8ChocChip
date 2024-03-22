//
// Created by doom on 19/03/24.
//

#include "Speaker.h"
#include <cmath>

Speaker::Speaker() {

    this->sound.setVolume(100.f);
    this->sound.setLoop(true);

    const unsigned int sampleRate = 44100;
    const unsigned int frequency = 440;
    sf::Int16 samples[sampleRate];
    for (unsigned int i = 0; i < sampleRate; ++i) {
        samples[i] = static_cast<sf::Int16>(32767 * std::sin(2 * 3.14159265 * frequency * i / sampleRate)); // Mmmm yum pi
    }
    this->soundBuffer.loadFromSamples(samples, sampleRate, 1, sampleRate);

    this->sound.setBuffer(this->soundBuffer);
}

void Speaker::play() {
    this->sound.play();
}

void Speaker::stop() {
    this->sound.stop();
}

