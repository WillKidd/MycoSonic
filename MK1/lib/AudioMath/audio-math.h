#ifndef AUDIO_MATH_H
#define AUDIO_MATH_H

#include <Arduino.h>
#include <math.h>

// Constants
const float A4_FREQ = 440.0;
const float SEMITONE_RATIO = 1.059463094359; // 2^(1/12)
const uint8_t MIDI_NOTE_A4 = 69;
const uint8_t MIN_MIDI_NOTE = 21;  // A0, lowest note on standard piano
const uint8_t MAX_MIDI_NOTE = 108; // C8, highest note on standard piano
const uint8_t MAX_SCALE_NOTES = 100; // Estimate for max number of notes needed

// Enum to specify the mapping type
enum MappingType {
    MAP_TO_FULL_SPECTRUM,
    MAP_TO_SCALE,
    DYNAMIC_RANGE_COMPRESSION,
    HARMONIC_MAPPING
};

// Function Declarations
float midiNoteToFrequency(uint8_t midiNumber);
uint8_t frequencyToMIDINote(float frequency);
float mapToScale(uint16_t input, uint16_t inputMin, uint16_t inputMax, uint8_t keyRootMidiNote, const uint8_t intervals[], uint8_t numIntervals);
float mapToFullSpectrum(uint16_t input, uint16_t inputMin, uint16_t inputMax);
float dynamicRangeCompressionMapping(uint16_t input, uint16_t inputMin, uint16_t inputMax, const uint8_t intervals[], uint8_t numIntervals);
float harmonicMapping(uint16_t input, uint16_t inputMin, uint16_t inputMax, float baseFrequency);
float mapInputToFrequency(uint16_t input, uint16_t inputMin, uint16_t inputMax, const uint8_t intervals[], uint8_t numIntervals, MappingType mappingType, float baseFrequency);
float millisecondsPerBeat(uint8_t bpm);
float noteDurationToTime(uint8_t noteType, uint8_t bpm, uint8_t beatUnit, uint8_t defaultBeatUnit);
float noteDurationToTimeFractional(float noteTypeRatio, uint8_t bpm, uint8_t beatUnit, uint8_t defaultBeatUnit);
#endif
