#include <MozziGuts.h>
#include <Oscil.h> // oscillator
#include "tables/sin2048_int8.h"


Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> oscil1;

void setup() {
  startMozzi(); // initialize Mozzi
  oscil1.setFreq(440); // set frequency to 440 Hz (A4 note)
}

void updateControl() {
}

int updateAudio() {
  return oscil1.next(); // output next value of the sine wave
}

void loop() {
  audioHook(); // continuously call audioHook()
}

