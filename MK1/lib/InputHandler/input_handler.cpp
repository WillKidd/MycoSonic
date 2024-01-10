#include "INPUT_HANDLER.h"

InputHandler::InputHandler(int okPin, int editPin, int backPin, int togglePin, int rotaryA, int rotaryB)
    : okButtonPin(okPin), editButtonPin(editPin), backButtonPin(backPin), toggleButtonPin(togglePin),
      rotaryPinA(rotaryA), rotaryPinB(rotaryB), 
      lastRotaryState(LOW), okPressed(false), editPressed(false),
      backPressed(false), togglePressed(false), 
      rotaryIncremented(false), rotaryDecremented(false) {

    pinMode(okButtonPin, INPUT);
    pinMode(editButtonPin, INPUT);
    pinMode(backButtonPin, INPUT);
    pinMode(toggleButtonPin, INPUT);
    pinMode(rotaryPinA, INPUT);
    pinMode(rotaryPinB, INPUT);
}

void InputHandler::update() {
    readButtons();
    readRotaryEncoder();
}

void InputHandler::readButtons() {
    okPressed = digitalRead(okButtonPin) == HIGH;
    editPressed = digitalRead(editButtonPin) == HIGH;
    backPressed = digitalRead(backButtonPin) == HIGH;
    togglePressed = digitalRead(toggleButtonPin) == HIGH;
}

void InputHandler::readRotaryEncoder() {
    int newState = digitalRead(rotaryPinA);
    if (newState != lastRotaryState) {
        lastRotaryState = newState;
        if (digitalRead(rotaryPinB) != newState) {
            rotaryIncremented = true;
        } else {
            rotaryDecremented = true;
        }
    } else {
        rotaryIncremented = false;
        rotaryDecremented = false;
    }
}

bool InputHandler::isOkPressed() const {
    return okPressed;
}

bool InputHandler::isEditPressed() const {
    return editPressed;
}

bool InputHandler::isBackPressed() const {
    return backPressed;
}

bool InputHandler::isToggleButtonPressed() const {
    return togglePressed;
}

bool InputHandler::isRotaryIncremented() const {
    return rotaryIncremented;
}

bool InputHandler::isRotaryDecremented() const {
    return rotaryDecremented;
}