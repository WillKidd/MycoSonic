#include <Arduino.h>
#include "signal_processing.h"
//#include "data_handler.h"
#include <SPI.h>
#include "mozzi-audio.h"
#include "audio-math.h"
#include "input_handler.h"
#include "menu_system.h"

const int bioSensorPin = A0;

bool useSensorData = true;
bool useSDCardData = false;

bool useMovingAverage = true;
bool useLowPass = false;
bool useHighPass = false;
bool useNotch = false;
bool useMedian = false;
bool useKalman = false;
int filterIndex = 0;

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

const int OK_BUTTON_PIN = 5;
const int EDIT_BUTTON_PIN = 4;
const int BACK_BUTTON_PIN = 2;
const int TOGGLE_BUTTON_PIN = 6;
const int ROTARY_PIN_A = 7;
const int ROTARY_PIN_B = 8;

InputHandler inputHandler(OK_BUTTON_PIN, EDIT_BUTTON_PIN, BACK_BUTTON_PIN, TOGGLE_BUTTON_PIN, ROTARY_PIN_A, ROTARY_PIN_B);
MenuItem rootItem("MycoSonic MK1", BASE_ITEM);
MenuItem inputItem("Input", BASE_ITEM);
ToggleMenuItem sensorItem("Sensor", useSensorData, SINGLE_TOGGLE_ITEM);
ToggleMenuItem sdItem("SD", useSDCardData, SINGLE_TOGGLE_ITEM);
MenuItem inputFilterItem("InputFilter", BASE_ITEM);
ToggleMenuItem movingAverageItem("MA", useSensorData, SINGLE_TOGGLE_ITEM);
ToggleMenuItem lowPassItem("LPF", useSDCardData, SINGLE_TOGGLE_ITEM);
ToggleMenuItem highPassItem("HPF", useSensorData, SINGLE_TOGGLE_ITEM);
ToggleMenuItem notchItem("NF", useSDCardData, SINGLE_TOGGLE_ITEM);
ToggleMenuItem medianItem("Mdn", useSensorData, SINGLE_TOGGLE_ITEM);
ToggleMenuItem kalmanItem("Kalman", useSDCardData, SINGLE_TOGGLE_ITEM);
MenuItem timingItem("Timing", BASE_ITEM);
MenuItem signalMapping("NoteMapping", BASE_ITEM); 
MenuItem outputFilterItem("OutputFilter", BASE_ITEM);
MenuItem outputItem("Output", BASE_ITEM);
MenuHandler menuHandler(&rootItem);


void setup() {
  Serial.begin(9600);
  //initSDCard();
  startMozzi(64);

  noteDuration = noteDurationToTime(noteDurationType, bpm, beatUnit, defaultBeatUnit);
  breakDuration = noteDurationToTime(breakDurationType, bpm, beatUnit, defaultBeatUnit);

  currentWaveform = new SawWave(currentFrequency);
  lastChangeTime = millis();
  LCDHandler lcdHandler(0x27, 16, 2);
  lcdHandler.init();
  
  rootItem.addChild(&inputItem);
  inputItem.addChild(&sensorItem);
  inputItem.addChild(&sdItem);
  rootItem.addChild(&inputFilterItem);
  inputFilterItem.addChild(&movingAverageItem);
  inputFilterItem.addChild(&lowPassItem);
  inputFilterItem.addChild(&highPassItem);
  inputFilterItem.addChild(&notchItem);
  inputFilterItem.addChild(&medianItem);
  inputFilterItem.addChild(&kalmanItem);
  rootItem.addChild(&timingItem);
  rootItem.addChild(&signalMapping); 
  rootItem.addChild(&outputFilterItem);
  rootItem.addChild(&outputItem);
  menuHandler.setLCDHandler(&lcdHandler);
  menuHandler.selectItem();
  menuHandler.displayCurrentItem();
  //Serial.println(inputItem.getName());
}

void loop(){
  audioHook();
}


void updateControl(){
  unsigned long currentTime = millis();
  
  inputHandler.update();
  if (inputHandler.isOkPressed()) {
    //Serial.println("OK");
    if (menuHandler.isEditMode()) {
        menuHandler.saveChanges(); // Save changes if in edit mode
        menuHandler.exitEditMode(); // Exit edit mode
    }
    //Serial.println(menuHandler.getCurrentItem()->getName());
    menuHandler.displayCurrentItem();
  }
  else if (inputHandler.isEditPressed()) {
      //Serial.println("EDIT");
      menuHandler.enterEditMode(); // Enter edit mode
      menuHandler.selectItem();
      //Serial.println(menuHandler.getCurrentItem()->getName());
      menuHandler.displayCurrentItem();
  }
  else if (inputHandler.isBackPressed()) {
      //Serial.println("BACK");
    if (menuHandler.isEditMode()) {
        menuHandler.discardChanges(); // Discard changes if in edit mode
        menuHandler.exitEditMode(); // Exit edit mode
        menuHandler.back();
    } else {
        menuHandler.back(); // Navigate back in the menu
    }
    //Serial.println(menuHandler.getCurrentItem()->getName());
    menuHandler.displayCurrentItem();
  }
  else if (inputHandler.isToggleButtonPressed()) {
    //Serial.println("TOGGLE");
    if (!menuHandler.isEditMode()){
      menuHandler.toggleCurrentItem(); // Toggle the current item
    }
    //Serial.println(menuHandler.getCurrentItem()->getName());
    menuHandler.displayCurrentItem();
  }
 else if (inputHandler.isUpButtonPressed()) {
    if (menuHandler.isEditMode()) {
      menuHandler.updateParameterValue(0.1);
    } 
    else {
      menuHandler.previousItem();
    }
    //Serial.println(menuHandler.getCurrentItem()->getName());
    menuHandler.displayCurrentItem();
  }
  else if (inputHandler.isDownButtonPressed()) {
    if (menuHandler.isEditMode()) {
      menuHandler.updateParameterValue(-0.1);
    } 
    else {
      menuHandler.nextItem();
    }
    //Serial.println(menuHandler.getCurrentItem()->getName());
    menuHandler.displayCurrentItem();
  }
  
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
        //currentWaveform->setFrequency(freq);
        currentWaveform->setFrequency(110); //for testing not as annoying
        currentState = PLAYING;
        lastChangeTime = currentTime;
      }
      break;
  }
}

int updateAudio(){
  return currentWaveform->update();
}