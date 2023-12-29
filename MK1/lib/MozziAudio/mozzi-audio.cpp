#include "mozzi-audio.h"

Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> sinewaveOscillator(SIN2048_DATA);
Oscil<TRIANGLE2048_NUM_CELLS, AUDIO_RATE> trianglewaveOscillator(TRIANGLE2048_DATA);
Oscil<SAW2048_NUM_CELLS, AUDIO_RATE> sawwaveOscillator(SAW2048_DATA);
Oscil<SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> squarewaveOscillator(SQUARE_NO_ALIAS_2048_DATA);

// Sine
SineWave::SineWave(int frequency) {
    sinewaveOscillator.setFreq(frequency);
}

int SineWave::update() {
    return sinewaveOscillator.next();
}

void SineWave::setFrequency(int frequency) {
    sinewaveOscillator.setFreq(frequency);
}

Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> SineWave::getOscillator(){
    return sinewaveOscillator;
}

// Triangle
TriangleWave::TriangleWave(int frequency) {
    trianglewaveOscillator.setFreq(frequency);
}

int TriangleWave::update() {
    return trianglewaveOscillator.next();
}

void TriangleWave::setFrequency(int frequency) {
    trianglewaveOscillator.setFreq(frequency);
}

Oscil<TRIANGLE2048_NUM_CELLS, AUDIO_RATE> TriangleWave::getOscillator(){
    return trianglewaveOscillator;
}

// SawTooth
SawWave::SawWave(int frequency) {
    sawwaveOscillator.setFreq(frequency);
}

int SawWave::update() {
    return sawwaveOscillator.next();
}

void SawWave::setFrequency(int frequency) {
    squarewaveOscillator.setFreq(frequency);
}

Oscil<SAW2048_NUM_CELLS, AUDIO_RATE> SawWave::getOscillator(){
    return sawwaveOscillator;
}

// Square
SquareWave::SquareWave(int frequency) {
    squarewaveOscillator.setFreq(frequency);
}

int SquareWave::update() {
    return squarewaveOscillator.next();
}

void SquareWave::setFrequency(int frequency) {
    squarewaveOscillator.setFreq(frequency);
}

Oscil<SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> SquareWave::getOscillator(){
    return squarewaveOscillator;
}

void switchWaveform(int waveformType) {
    extern BaseWaveform* currentWaveform;
    extern int currentFrequency;

    if (currentWaveform != nullptr) {
        delete currentWaveform;  // Free the old waveform object
    }

    switch(waveformType) {
        case 0:
            currentWaveform = new SineWave(currentFrequency);
            break;
        case 1:
            currentWaveform = new TriangleWave(currentFrequency);
            break;
        case 2:
            currentWaveform = new SawWave(currentFrequency);
            break;
        case 3:
            currentWaveform = new SquareWave(currentFrequency);
            break;
        default:
            currentWaveform = new SineWave(currentFrequency); 
    }
}