#include <Arduino.h>
#include "signal_processing.h"
#include "data_handler.h"
#include "mozzi-audio.h"
#include "audio-math.h"
#include "input_handler.h"
#include "menu_system.h"

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

// User-settable note and break durations (as note types, e.g., 4 for quarter note)
int noteDurationType = 4; // Quarter note
int breakDurationType = 4; // Quarter note

float noteDuration; // Duration of a note in milliseconds
float breakDuration; // Duration of a break in milliseconds

enum State { PLAYING, BREAK };
State currentState = PLAYING;

BaseWaveform* currentWaveform = nullptr;
int currentFrequency = 220;

const int OK_BUTTON_PIN = 2;
const int EDIT_BUTTON_PIN = 3;
const int BACK_BUTTON_PIN = 4;
const int TOGGLE_BUTTON_PIN = 5;
const int ROTARY_PIN_A = 6;
const int ROTARY_PIN_B = 7;

InputHandler inputHandler(OK_BUTTON_PIN, EDIT_BUTTON_PIN, BACK_BUTTON_PIN, TOGGLE_BUTTON_PIN, ROTARY_PIN_A, ROTARY_PIN_B);
MenuItem rootItem("Main Menu", BASE_ITEM);
MenuHandler menuHandler(&rootItem); // Assuming this is already created
LCDHandler lcdHandler(0x27, 16, 2); // Assuming this is already created


void setup() {
  Serial.begin(9600);
  initSDCard();
  startMozzi(64);
  // Calculate durations based on user-set values
  noteDuration = noteDurationToTime(noteDurationType, bpm, beatUnit, defaultBeatUnit);
  //breakDuration = 0.0;
  breakDuration = noteDurationToTime(breakDurationType, bpm, beatUnit, defaultBeatUnit);

  currentWaveform = new SawWave(currentFrequency);
  lastChangeTime = millis();
}

void loop(){
  audioHook();
}

void updateControl(){
  unsigned long currentTime = millis();
  switch (currentState) {
    case PLAYING:
      if (currentTime - lastChangeTime >= noteDuration) {
        currentWaveform->setFrequency(0); // Silence the output for a break
        currentState = BREAK;
        lastChangeTime = currentTime;
      }
      break;
    case BREAK:
      if (currentTime - lastChangeTime >= breakDuration) {
        bioValue = analogRead(bioSensorPin);
        dataValue = applyFilter(bioValue, filterIndex);
        float freq = harmonicMapping(dataValue, 0, 1023, 110);
        currentWaveform->setFrequency(freq);
        currentState = PLAYING;
        lastChangeTime = currentTime;
      }
      break;
  }

  inputHandler.update();
  if (inputHandler.isOkPressed()) {
    if (menuHandler.isEditMode()) {
        menuHandler.saveChanges(); // Save changes if in edit mode
        menuHandler.exitEditMode(); // Exit edit mode
    }
  }

  // Handle Edit Button Press - Toggle Edit Mode
  if (inputHandler.isEditPressed()) {
      if (menuHandler.isEditMode()) {
          menuHandler.exitEditMode(); // Exit edit mode
      } else {
          menuHandler.enterEditMode(); // Enter edit mode
      }
  }

  // Handle Back Button Press - Discard Changes or Navigate Back
  if (inputHandler.isBackPressed()) {
      if (menuHandler.isEditMode()) {
          menuHandler.discardChanges(); // Discard changes if in edit mode
          menuHandler.exitEditMode(); // Exit edit mode
      } else {
          menuHandler.back(); // Navigate back in the menu
      }
  }

  // Handle Toggle Button Press - Toggle Current Item
  if (inputHandler.isToggleButtonPressed()) {
      menuHandler.toggleCurrentItem(); // Toggle the current item
  }

  // Handle Rotary Encoder Increments
  if (inputHandler.isRotaryIncremented()) {
      if (menuHandler.isEditMode()) {
          menuHandler.updateParameterValue(0.1); // Increase editable parameter
      } else {
          menuHandler.nextItem(); // Navigate to the next item
      }
  }

  // Handle Rotary Encoder Decrements
  if (inputHandler.isRotaryDecremented()) {
      if (menuHandler.isEditMode()) {
          menuHandler.updateParameterValue(-0.1); // Decrease editable parameter
      } else {
          menuHandler.previousItem(); // Navigate to the previous item
      }
  }
}

int updateAudio(){
  return currentWaveform->update();
}

