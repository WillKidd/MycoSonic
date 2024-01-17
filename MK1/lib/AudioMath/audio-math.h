#ifndef AUDIO_MATH_H
#define AUDIO_MATH_H

#include <Arduino.h>
#include <math.h>

// Constants
const uint16_t A4_FREQ = 440;
const float SEMITONE_RATIO = 1.059463094359; // 2^(1/12)
const uint8_t MIDI_NOTE_A4 = 69;
const uint8_t MIN_MIDI_NOTE = 21;  // A0, lowest note on standard piano
const uint8_t MAX_MIDI_NOTE = 84; // C8, highest note on standard piano
const uint8_t MAX_SCALE_NOTES = 100; // Estimate for max number of notes needed

// Enum to specify the mapping type
enum MappingType {
    MAP_TO_FULL_SPECTRUM,
    MAP_TO_SCALE,
    DYNAMIC_RANGE_COMPRESSION,
    HARMONIC_MAPPING
};

// Function Declarations
uint16_t inputToMidiNote(uint16_t input, uint16_t inputMin, uint16_t inputMax);
//uint16_t midiNoteToFrequency(uint8_t midiNumber);
uint8_t frequencyToMIDINote(float frequency);
uint16_t mapToScale(uint16_t input, uint16_t inputMin, uint16_t inputMax, uint8_t keyRootMidiNote, const uint8_t intervals[], uint8_t numIntervals);
uint16_t mapToFullSpectrum(uint16_t input, uint16_t inputMin, uint16_t inputMax);
uint16_t dynamicRangeCompressionMapping(uint16_t input, uint16_t inputMin, uint16_t inputMax, const uint8_t intervals[], uint8_t numIntervals);
uint16_t harmonicMapping(uint16_t input, uint16_t inputMin, uint16_t inputMax, uint8_t baseFrequencyMidiNote);
float millisecondsPerBeat(uint8_t bpm);
float noteDurationToTime(uint8_t noteType, uint8_t bpm, uint8_t beatUnit, uint8_t defaultBeatUnit);
float noteDurationToTimeFractional(float noteTypeRatio, uint8_t bpm, uint8_t beatUnit, uint8_t defaultBeatUnit);
#endif
