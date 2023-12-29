#include <Arduino.h>
#include "signal_processing.h"
#include "data_logging.h"
#include "mozzi-audio.h"
#include "audio-math.h"


const int bioSensorPin = A0;
int filterIndex = 0;
bool useSDCardData = false;
bool logDataToSDCard = false;

float dataValue;
int bioValue;

unsigned long lastChangeTime;
bool isNotePlaying = true;
int bpm = 120; // Example BPM
int beatUnit = 4; // Assuming 4/4 time signature
int defaultBeatUnit = 4; // Default beat unit for the time signature
int topNumber = 4; // Top number of the time signature (e.g., 4 in 4/4 time)
int currentBeat = 0; // Current beat within the measure

// User-settable note and break durations (as note types, e.g., 4 for quarter note)
int noteDurationType = 4; // Quarter note
int breakDurationType = 8; // Eighth note

float noteDuration; // Duration of a note in milliseconds
float breakDuration; // Duration of a break in milliseconds

enum State { PLAYING, BREAK };
State currentState = PLAYING;

void setup() {
  Serial.begin(9600);
  initSDCard();
  startMozzi(64);
  initializeSineWave(220);

  // Calculate durations based on user-set values
  noteDuration = noteDurationToTime(noteDurationType, bpm, beatUnit, defaultBeatUnit);
  breakDuration = noteDurationToTime(breakDurationType, bpm, beatUnit, defaultBeatUnit);

  lastChangeTime = millis();
}

void loop(){
  audioHook();
  updateControl();
}

void updateControl(){
  unsigned long currentTime = millis();
  switch (currentState) {
    case PLAYING:
      if (millis() - lastChangeTime >= noteDuration) {
        currentBeat++;  // Increment the current beat count

        // Check if the end of the measure is reached
        if (currentBeat >= topNumber) {
          currentBeat = 0;  // Reset beat count at the start of a new measure
          // Optionally, implement actions that should occur at the start of a new measure
        }

        // Check if breaks are implemented and have duration
        if (breakDuration > 0) {
          setSineWaveFrequency(0);  // Silence the output for a break
          currentState = BREAK;     // Change state to BREAK
        } else {
          // Start the next note if no break duration
          bioValue = analogRead(bioSensorPin);
          dataValue = applyFilter(bioValue, filterIndex);
          float freq = harmonicMapping(dataValue, 0, 1023, 220);
          setSineWaveFrequency(freq);  // Set frequency for the next note
          lastChangeTime = millis();   // Update the time of the last change
        }
      }
      break;
    case BREAK:
      if (currentTime - lastChangeTime >= breakDuration) {
        bioValue = analogRead(bioSensorPin);
        dataValue = applyFilter(bioValue, filterIndex);
        float freq = harmonicMapping(dataValue, 0, 1023, 220);
        setSineWaveFrequency(freq);
        currentState = PLAYING;
        lastChangeTime = currentTime;
      }
      break;
  }
}

int updateAudio(){
  return updateSineWave();
}

// Add the bpmToMilliseconds and noteDurationToTime functions here

