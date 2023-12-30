#ifndef MOZZI_AUDIO_H
#define MOZZI_AUDIO_H

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/sin2048_int8.h>
#include <tables/triangle2048_int8.h>
#include <tables/saw2048_int8.h>
#include <tables/square_no_alias_2048_int8.h>

class BaseWaveform {
public:
    virtual int update() = 0;
    virtual void setFrequency(int frequency) = 0;
    virtual Oscil<2048U, AUDIO_RATE> getOscillator();
    virtual ~BaseWaveform() {}
};

class SineWave : public BaseWaveform {
public:
    SineWave(int frequency);
    int update() override;
    void setFrequency(int frequency) override;
    Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> getOscillator() override;
};

class TriangleWave : public BaseWaveform {
public:
    TriangleWave(int frequency);
    int update() override;
    void setFrequency(int frequency) override;
    Oscil<TRIANGLE2048_NUM_CELLS, AUDIO_RATE> getOscillator();
};

class SawWave : public BaseWaveform {
public:
    SawWave(int frequency);
    int update() override;
    void setFrequency(int frequency) override;
    Oscil<SAW2048_NUM_CELLS, AUDIO_RATE> getOscillator();
};

class SquareWave : public BaseWaveform {
public:
    SquareWave(int frequency);
    int update() override;
    void setFrequency(int frequency) override;
    Oscil<SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> getOscillator();
};

void switchWaveform(int waveformType);


class PhaseModulationEffect {
public:
    PhaseModulationEffect(float lfoRate, float modulationDepth);

    void updateLFO();
    int applyEffect(int inputSample);
    void setEffectEnabled(bool enabled);

private:
    float lfoRate;
    float modulationDepth;
    float lfoPhase;
    bool effectEnabled;  // New member variable to store the effect's state
};

class TremoloEffect {
public:
    TremoloEffect(float rate, float depth);
    void updateLFO();
    int applyEffect(int inputSample);
    void enableEffect(bool enable);

private:
    float rate;  // Speed of the tremolo modulation
    float depth; // Depth of the tremolo effect
    float lfoPhase;
    bool effectEnabled;
};

class BitCrusherEffect {
public:
    BitCrusherEffect(float bitDepth, float reducedSampleRate);
    int applyEffect(int inputSample);
    void enableEffect(bool enable);

private:
    float bitDepth;          // Desired bit depth
    float reducedSampleRate; // Reduced sample rate
    bool effectEnabled;
    unsigned long lastSampleTime;
    int lastSample;
};

class RingModulatorEffect {
public:
    RingModulatorEffect(float carrierFrequency);
    int applyEffect(int inputSample);
    void enableEffect(bool enable);

private:
    float carrierFrequency; // Frequency of the carrier wave
    float carrierPhase;
    bool effectEnabled;
};

class FlangerEffect {
public:
    FlangerEffect(float maxDelayMs, float rate);
    int applyEffect(int inputSample);
    void enableEffect(bool enable);
private:
    static constexpr float MAX_DELAY_MS = 10.0f;   // Maximum delay in milliseconds
    static const int DELAY_BUFFER_SIZE = static_cast<int>(AUDIO_RATE * MAX_DELAY_MS / 1000);
    float maxDelayMs;  // Maximum delay in milliseconds
    float rate;        // Rate of LFO
    float lfoPhase;
    int delayBuffer[DELAY_BUFFER_SIZE]; // Buffer for delayed samples
    int bufferIndex;
    bool effectEnabled;
};

#endif // MOZZI_AUDIO_H
