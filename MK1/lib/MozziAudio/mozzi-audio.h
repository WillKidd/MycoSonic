#ifndef MOZZI_AUDIO_H
#define MOZZI_AUDIO_H

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/sin2048_int8.h>

// Declare the oscillator for the sine wave
extern Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> sinewave1;

void initializeSineWave(int frequency);
int updateSineWave();
void setSineWaveFrequency(int frequency); // Function to set the frequency

#endif // MOZZI_AUDIO_H
