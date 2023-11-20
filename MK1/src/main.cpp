#include <Arduino.h>
#include <MIDI.h>

// Create and bind the MIDI interface to the default hardware Serial port
MIDI_CREATE_DEFAULT_INSTANCE();

// configuration
#define SENSOR_PIN A0
#define UNITS 1023
#define SAMPLING_RATE_PER_SECOND 1
#define MIDI_CHANNEL 1
// in V
#define INPUT_VOLTAGE 5.0 
// in Ohms
#define REFERENCE_RESISTANCE 100

// initialization
int sampling_delay_ms = 1000 / SAMPLING_RATE_PER_SECOND;
int measured_units = 0;
float measured_resistance = 0;

//function declarations
////related to calculating the resistance
float calculate_output_voltage(float, int);
float calculate_resistance(float, int);


/*
* below are representative midi values for their equal temperament note equivalent
* to go to the next highe octabe, add 12 to the representative midi value
Octave C C#/Db D D#/Eb E  F F#/Gb G G#/Ab A A#/Bb B
-2     0   1   2   3   4  5   6   7   8   9  10  11 
*/

void setup() {
  MIDI.begin(MIDI_CHANNEL);
  //MIDI.setHandleNoteOn(handleNoteOn);
  //MIDI.setHandleNoteOff(handleNoteOff);

  Serial.begin(115200);
  Serial.println("Arduino ready.");
}

void loop() {
  measured_units = analogRead(SENSOR_PIN);
  measured_resistance = calculate_resistance(INPUT_VOLTAGE, measured_units);
  Serial.println(measured_resistance);
  MIDI.sendNoteOn(72, 100, MIDI_CHANNEL);
  delay(sampling_delay_ms);
  MIDI.sendNoteOff(72, 100, MIDI_CHANNEL);
}

//function definitions
////related to calculating the resistance
float calculate_output_voltage(float input_voltage, int measured_units){
  float output_voltage = (input_voltage * measured_units) / UNITS;
  return output_voltage;
}

float calculate_resistance(float input_voltage, int measured_units){
  float output_voltage = calculate_output_voltage(input_voltage, measured_units);
  float resistance = REFERENCE_RESISTANCE * (1/((input_voltage/output_voltage) - 1));
  return resistance;
}


