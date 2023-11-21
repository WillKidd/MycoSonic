#include <Arduino.h>
#include <MIDI.h>

#define DEBUG_MODE true
// configuration
#define SENSOR_PIN A0
#define UNITS 1023
#define MIDI_CHANNEL 3
// can't suggest going over 8 per second -> problems with hairless midi or windows wavetable, sampling rate throttles amount of notes being played
#define SAMPLING_RATE_PER_SECOND 1
#define NOTE_DURATION_MS (int)((1/(SAMPLING_RATE_PER_SECOND))*1000)
#define SAMPLING_TIME_MS 1000/SAMPLING_RATE_PER_SECOND
#define MAX_NOTES 10 // Maximum number of concurrent notes

#define abs(x) ((x)>0?(x):-(x))
// in V
#define INPUT_VOLTAGE 5.0 
// in Ohms
#define REFERENCE_RESISTANCE 100
#define THRESHOLD 1

// Create and bind the MIDI interface to the default hardware Serial port
MIDI_CREATE_DEFAULT_INSTANCE();

struct Note {
  int number;          // MIDI note number
  unsigned long startTime; // Start time of the note
  bool isPlaying;      // Is the note currently playing?
};

Note notes[MAX_NOTES];

// initialization
int measured_units = 0;
float measured_resistance = 0;
float last_measured_resistance = 0;
float lowest_measured_resistance = 0;
float highest_measured_resistance = 0;

float totalResistance = 0;
int resistanceReadings = 0;
float absDifference = 0;

//function declarations
float calculate_output_voltage(float, int);
float calculate_resistance(float, int);
void play_note(int);
void updateAverageResistance(float);
float getAverageResistance();
int getDynamicNoteFromResistance(float);
int getDynamicNoteFromDifference(float);
void updateLowestAndHighestResistance(float);

/*
* below are representative midi values for their equal temperament note equivalent
* to go to the next highe octabe, add 12 to the representative midi value
Octave C C#/Db D D#/Eb E  F F#/Gb G G#/Ab A A#/Bb B
-2     0   1   2   3   4  5   6   7   8   9  10  11 
*/

void setup() {
MIDI.begin(MIDI_CHANNEL);
  Serial.begin(115200);
  Serial.println("Arduino ready.");

  // Initialize all notes as not playing
  for (int i = 0; i < MAX_NOTES; i++) {
    notes[i].isPlaying = false;
  }
}

void loop() {
  measured_units = analogRead(SENSOR_PIN);
  measured_resistance = calculate_resistance(INPUT_VOLTAGE, measured_units);

  updateAverageResistance(measured_resistance);
  //int noteToPlay = getDynamicNoteFromResistance(measured_resistance);
  float difference = last_measured_resistance - measured_resistance;
  absDifference = abs(difference);
  int noteToPlay = getDynamicNoteFromDifference(difference);

    if (DEBUG_MODE) {
      Serial.print("Measured Units: ");
      Serial.println(measured_units);
      Serial.print("Measured Resistance: ");
      Serial.println(measured_resistance);
      Serial.print("Average Resistance: ");
      Serial.println(getAverageResistance());
      Serial.print("Note To Play: ");
      Serial.println(noteToPlay);
  }

  if (absDifference > THRESHOLD){
      play_note(noteToPlay);
      last_measured_resistance = measured_resistance;
  }

  for (int i = 0; i < MAX_NOTES; i++) {
    if (notes[i].isPlaying && millis() - notes[i].startTime >= NOTE_DURATION_MS) {
      MIDI.sendNoteOff(notes[i].number, 100, MIDI_CHANNEL);
      notes[i].isPlaying = false;
    }
  }
  delay(SAMPLING_TIME_MS);
}

//function definitions
float calculate_output_voltage(float input_voltage, int measured_units){
  float output_voltage = (input_voltage * measured_units) / UNITS;
  return output_voltage;
}

float calculate_resistance(float input_voltage, int measured_units){
  float output_voltage = calculate_output_voltage(input_voltage, measured_units);
  float resistance = REFERENCE_RESISTANCE * (1/((input_voltage/output_voltage) - 1));
  return resistance;
}

void play_note(int inNoteNumber) {
  for (int i = 0; i < MAX_NOTES; i++) {
    if (!notes[i].isPlaying) {
      MIDI.sendNoteOn(inNoteNumber, 100, MIDI_CHANNEL);
      notes[i].number = inNoteNumber;
      notes[i].startTime = millis();
      notes[i].isPlaying = true;
      break;
    }
  }
}

void updateAverageResistance(float newResistance) {
  totalResistance += newResistance;
  resistanceReadings++;
}

// TODO: resistanceReadings can overflow -> gotta fix in next version
float getAverageResistance() {
  if (resistanceReadings == 0) return 0;
  return totalResistance / resistanceReadings;
}

int getDynamicNoteFromResistance(float resistance) {
  float averageResistance = getAverageResistance();
  // Adjust the note mapping based on the average resistance
  // This is just a stupid example, different kinds of mapping have great potential
  if (resistance < averageResistance * 0.8) {
    return 60;
  } else if (resistance < averageResistance * 1.0) {
    return 62;
  } else if (resistance < averageResistance * 1.2) {
    return 64;
  } else if (resistance < averageResistance * 1.4) {
    return 65;
  } else if (resistance < averageResistance * 1.6) {
    return 67;
  } else {
    return 69;
  }
}

int getDynamicNoteFromDifference(float difference){
  float averageResitance = getAverageResistance();
  float median_note = 64;
  float note_per_resistance = median_note / averageResitance;
  float interval = note_per_resistance * difference;
  int noteToPlay = median_note + interval;
  return noteToPlay;
}



