#include <math.h>

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

// Function to generate a scale with custom intervals
void generateScale(int rootMidiNote, const int intervals[], int size, float scaleFrequencies[]) {
    int currentNote = rootMidiNote;
    for(int i = 0; i < size; i++) {
        scaleFrequencies[i] = midiNoteToFrequency(currentNote);
        currentNote += intervals[i];
    }
}

// Function to generate a scale with custom intervals across the full spectrum
int generateFullSpectrumScale(const int intervals[], int numIntervals, float scaleNotes[]) {
    int noteCount = 0;

    for (int midiNumber = MIN_MIDI_NOTE; midiNumber <= MAX_MIDI_NOTE && noteCount < MAX_SCALE_NOTES;) {
        for (int i = 0; i < numIntervals && midiNumber <= MAX_MIDI_NOTE && noteCount < MAX_SCALE_NOTES; ++i) {
            scaleNotes[noteCount++] = midiNoteToFrequency(midiNumber);
            midiNumber += intervals[i];
        }
    }

    return noteCount; // Return the number of notes added to the array
}

/*
// Example usage:

float fullSpectrumCMajor[MAX_SCALE_NOTES];
int majorIntervals[] = {2, 2, 1, 2, 2, 2, 1}; // Intervals for a major scale
int numNotes = generateFullSpectrumScale(majorIntervals, 7, fullSpectrumCMajor);

start is index: 0
end is index: numNotes-1
*/
