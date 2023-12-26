#include "audio-math.h"

#define A4_FREQ 440.0
#define SEMITONE_RATIO 1.059463094359 // 2^(1/12)
#define MIDI_NOTE_A4 69
#define MIN_MIDI_NOTE 21  // A0, lowest note on standard piano
#define MAX_MIDI_NOTE 108 // C8, highest note on standard piano
#define MAX_SCALE_NOTES 100 // Estimate for max number of notes needed

float midiNoteToFrequency(int midiNumber) {
    // Bounds checking
    if (midiNumber < MIN_MIDI_NOTE) midiNumber = MIN_MIDI_NOTE;
    if (midiNumber > MAX_MIDI_NOTE) midiNumber = MAX_MIDI_NOTE;

    // Calculate frequency
    return A4_FREQ * pow(SEMITONE_RATIO, midiNumber - MIDI_NOTE_A4);
}

// Function to calculate MIDI note number from frequency
int frequencyToMIDINote(float frequency) {
    // Handle non-positive frequencies by returning the lowest possible MIDI note
    if (frequency <= 0) return MIN_MIDI_NOTE;

    // Calculate the MIDI note number using log (natural logarithm)
    int midiNote = round(MIDI_NOTE_A4 + (12 * log(frequency / A4_FREQ) / log(2)));

    // Clamp the MIDI note to the valid range
    if (midiNote < MIN_MIDI_NOTE) midiNote = MIN_MIDI_NOTE;
    if (midiNote > MAX_MIDI_NOTE) midiNote = MAX_MIDI_NOTE;

    return midiNote;
}

// Enum to specify the mapping type
enum MappingType {
    MAP_TO_SCALE,
    MAP_TO_FULL_SPECTRUM
};

// Function to map an input value to a note frequency
float mapInputToFrequency(int input, int inputMin, int inputMax, const int intervals[], int numIntervals, MappingType mappingType) {
    if (mappingType == MAP_TO_FULL_SPECTRUM) {
        // Map directly to the full MIDI spectrum
        int midiNumber = map(input, inputMin, inputMax, MIN_MIDI_NOTE, MAX_MIDI_NOTE);
        return midiNoteToFrequency(midiNumber);
    } else {
        // Map to a specific scale
        int totalNotes = MAX_MIDI_NOTE - MIN_MIDI_NOTE + 1;
        int spectrumPosition = map(input, inputMin, inputMax, 0, totalNotes - 1);

        int midiNumber = MIN_MIDI_NOTE;
        while (spectrumPosition > 0) {
            for (int i = 0; i < numIntervals && spectrumPosition > 0; ++i, --spectrumPosition) {
                midiNumber += intervals[i % numIntervals];
                if (midiNumber > MAX_MIDI_NOTE) {
                    midiNumber = MIN_MIDI_NOTE + (midiNumber - MAX_MIDI_NOTE - 1);
                }
            }
        }

        return midiNoteToFrequency(midiNumber);
    }
}

/*
// Example usage:

int majorIntervals[] = {2, 2, 1, 2, 2, 2, 1}; // Intervals for a major scale
int sensorValue = analogRead(A0); // Read a value from an analog sensor

// Map the sensor value to a frequency in the major scale
float frequencyScale = mapInputToFrequency(sensorValue, 0, 1023, majorIntervals, 7, MAP_TO_SCALE);

// Map the sensor value to a frequency across the full MIDI spectrum
float frequencyFullSpectrum = mapInputToFrequency(sensorValue, 0, 1023, majorIntervals, 7, MAP_TO_FULL_SPECTRUM);

*/
