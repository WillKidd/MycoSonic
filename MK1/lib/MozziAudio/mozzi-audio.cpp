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

FlangerEffect::FlangerEffect(float maxDelayMs, float rate)
    : maxDelayMs(maxDelayMs), rate(rate), lfoPhase(0), bufferIndex(0), effectEnabled(true) {
    memset(delayBuffer, 0, sizeof(delayBuffer));
}

int FlangerEffect::applyEffect(int inputSample) {
    if (!effectEnabled) {
        return inputSample;
    }

    // Update LFO phase
    lfoPhase += rate / AUDIO_RATE;
    if (lfoPhase > 1) lfoPhase -= 1;

    // Calculate delay offset
    float lfoValue = (sin(lfoPhase * 2 * PI) + 1) / 2;  // Normalized to 0-1
    int delayOffset = static_cast<int>(lfoValue * maxDelayMs * AUDIO_RATE / 1000);

    // Add delayed sample to input sample
    int delayedSampleIndex = bufferIndex - delayOffset;
    if (delayedSampleIndex < 0) delayedSampleIndex += DELAY_BUFFER_SIZE;
    int outputSample = (inputSample + delayBuffer[delayedSampleIndex]) / 2;

    // Update delay buffer
    delayBuffer[bufferIndex] = inputSample;
    bufferIndex = (bufferIndex + 1) % DELAY_BUFFER_SIZE;

    return outputSample;
}

void FlangerEffect::enableEffect(bool enable) {
    effectEnabled = enable;
}

PitchShifterEffect::PitchShifterEffect(float pitchShiftFactor)
    : pitchShiftFactor(pitchShiftFactor), readIndex(0), writeIndex(0), effectEnabled(true) {
    memset(delayBuffer, 0, sizeof(delayBuffer));
}

int PitchShifterEffect::applyEffect(int inputSample) {
    if (!effectEnabled) {
        return inputSample;
    }

    // Store the current sample in the buffer
    delayBuffer[writeIndex] = inputSample;

    // Calculate the read index
    readIndex = writeIndex - static_cast<int>(pitchShiftFactor * PITCH_BUFFER_SIZE);
    if (readIndex < 0) readIndex += PITCH_BUFFER_SIZE;

    // Read the shifted sample
    int shiftedSample = delayBuffer[readIndex];

    // Update the write index
    writeIndex = (writeIndex + 1) % PITCH_BUFFER_SIZE;

    return shiftedSample;
}

void PitchShifterEffect::enableEffect(bool enable) {
    effectEnabled = enable;
}

DistortionEffect::DistortionEffect(float gain, float threshold)
    : gain(gain), threshold(threshold), effectEnabled(true) {}

int DistortionEffect::applyEffect(int inputSample) {
    if (!effectEnabled) {
        return inputSample;
    }

    float amplifiedSample = inputSample * gain;

    // Clipping
    if (amplifiedSample > threshold) {
        amplifiedSample = threshold;
    } else if (amplifiedSample < -threshold) {
        amplifiedSample = -threshold;
    }

    return static_cast<int>(amplifiedSample);
}

void DistortionEffect::enableEffect(bool enable) {
    effectEnabled = enable;
}

PanEffect::PanEffect(float lfoRate)
    : lfoRate(lfoRate), lfoPhase(0), effectEnabled(true) {}

void PanEffect::applyEffect(int& leftChannel, int& rightChannel) {
    if (!effectEnabled) {
        return;
    }

    // Update LFO phase
    lfoPhase += lfoRate / AUDIO_RATE;
    if (lfoPhase > 1) lfoPhase -= 1;

    // Calculate panning value (from -1 to 1)
    float pan = sin(lfoPhase * 2 * PI);

    // Adjust the left and right channels based on the panning value
    float leftGain = pan <= 0 ? 1 : 1 - pan;
    float rightGain = pan >= 0 ? 1 : 1 + pan;

    leftChannel = static_cast<int>(leftChannel * leftGain);
    rightChannel = static_cast<int>(rightChannel * rightGain);
}

void PanEffect::enableEffect(bool enable) {
    effectEnabled = enable;
}

VibratoEffect::VibratoEffect(float depth, float rate)
    : depth(depth), rate(rate), lfoPhase(0), effectEnabled(true) {}

int VibratoEffect::applyEffect(int inputSample) {
    if (!effectEnabled) {
        return inputSample;
    }

    // Update LFO phase
    lfoPhase += rate / AUDIO_RATE;
    if (lfoPhase > 1) lfoPhase -= 1;

    // Modulate the pitch using the LFO
    float vibrato = sin(lfoPhase * 2 * PI) * depth;

    // For a real implementation, you would need to adjust the sample rate or use a pitch-shifting algorithm
    // Here, we'll simulate it by simply modulating the sample value
    int modulatedSample = static_cast<int>(inputSample * (1 + vibrato));

    return modulatedSample;
}

void VibratoEffect::enableEffect(bool enable) {
    effectEnabled = enable;
}

LeslieEffect::LeslieEffect(float speed)
    : speed(speed), phase(0), effectEnabled(true) {}

int LeslieEffect::applyEffect(int inputSample) {
    if (!effectEnabled) {
        return inputSample;
    }

    // Update phase
    phase += speed / AUDIO_RATE;
    if (phase > 1) phase -= 1;

    // Modulate the sound
    float modulation = sin(phase * 2 * PI);
    int modulatedSample = static_cast<int>(inputSample * (1 + modulation));

    return modulatedSample;
}

void LeslieEffect::enableEffect(bool enable) {
    effectEnabled = enable;
}