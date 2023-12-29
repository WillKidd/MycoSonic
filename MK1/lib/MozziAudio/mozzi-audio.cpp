#include "mozzi-audio.h"

// Define the sine wave oscillator
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> sinewave1(SIN2048_DATA);

void initializeSineWave(int frequency) {
    sinewave1.setFreq(frequency);
}

int updateSineWave() {
    return sinewave1.next();
}

void setSineWaveFrequency(int frequency) {
    sinewave1.setFreq(frequency); // Set the new frequency
}
