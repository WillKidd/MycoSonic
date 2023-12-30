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
    sawwaveOscillator.setFreq(frequency);
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

PhaseModulationEffect::PhaseModulationEffect(float lfoRate, float modulationDepth)
    : lfoRate(lfoRate), modulationDepth(modulationDepth), lfoPhase(0) {}

void PhaseModulationEffect::updateLFO() {
    lfoPhase += lfoRate / AUDIO_RATE; 
    if (lfoPhase > 1.0) {
        lfoPhase -= 1.0;
    }
}

int PhaseModulationEffect::applyEffect(int inputSample) {
    if (!effectEnabled){
        return inputSample;
    }
    float modulation = sin(lfoPhase * 2 * PI) * modulationDepth;
    return static_cast<int>(inputSample * (1 + modulation));
}

void PhaseModulationEffect::setEffectEnabled(bool enabled){
    effectEnabled = enabled;
}

TremoloEffect::TremoloEffect(float rate, float depth)
    : rate(rate), depth(depth), lfoPhase(0), effectEnabled(true) {}

void TremoloEffect::updateLFO() {
    lfoPhase += rate / AUDIO_RATE;
    if (lfoPhase > 1) lfoPhase -= 1;
}

int TremoloEffect::applyEffect(int inputSample) {
    if (!effectEnabled) {
        return inputSample;
    }
    float lfoValue = (sin(lfoPhase * 2 * PI) + 1) / 2;  // Normalized to 0 to 1
    float modulation = 1 - (depth * lfoValue);
    return static_cast<int>(inputSample * modulation);
}

void TremoloEffect::enableEffect(bool enable) {
    effectEnabled = enable;
}

BitCrusherEffect::BitCrusherEffect(float bitDepth, float reducedSampleRate)
    : bitDepth(bitDepth), reducedSampleRate(reducedSampleRate), effectEnabled(true), lastSampleTime(0), lastSample(0) {}

int BitCrusherEffect::applyEffect(int inputSample) {
    if (!effectEnabled) {
        return inputSample;
    }
    
    unsigned long currentTime = micros();
    if (currentTime - lastSampleTime > 1000000 / reducedSampleRate) {
        lastSampleTime = currentTime;
        lastSample = inputSample & (~((1 << (16 - static_cast<int>(bitDepth))) - 1));
    }

    return lastSample;
}

void BitCrusherEffect::enableEffect(bool enable) {
    effectEnabled = enable;
}

RingModulatorEffect::RingModulatorEffect(float carrierFrequency)
    : carrierFrequency(carrierFrequency), carrierPhase(0), effectEnabled(true) {}

int RingModulatorEffect::applyEffect(int inputSample) {
    if (!effectEnabled) {
        return inputSample;
    }

    // Update the carrier phase
    carrierPhase += (2 * PI * carrierFrequency) / AUDIO_RATE;
    if (carrierPhase > 2 * PI) {
        carrierPhase -= 2 * PI;
    }

    // Apply ring modulation effect
    float carrierSignal = sin(carrierPhase);
    return static_cast<int>(inputSample * carrierSignal);
}

void RingModulatorEffect::enableEffect(bool enable) {
    effectEnabled = enable;
}