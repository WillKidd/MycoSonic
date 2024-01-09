#include "INPUT_HANDLER.h"

InputHandler::InputHandler(int okPin, int editPin, int backPin, int togglePin, int rotaryA, int rotaryB)
    : okButtonPin(okPin), editButtonPin(editPin), backButtonPin(backPin), toggleButtonPin(togglePin),
      rotaryPinA(rotaryA), rotaryPinB(rotaryB), 
      lastRotaryState(LOW), okPressed(false), editPressed(false),
      backPressed(false), togglePressed(false), 
      rotaryIncremented(false), rotaryDecremented(false) {

    pinMode(okButtonPin, INPUT_PULLUP);
    pinMode(editButtonPin, INPUT_PULLUP);
    pinMode(backButtonPin, INPUT_PULLUP);
    pinMode(toggleButtonPin, INPUT_PULLUP);
    pinMode(rotaryPinA, INPUT);
    pinMode(rotaryPinB, INPUT);
}

void InputHandler::update() {
    readButtons();
    readRotaryEncoder();
}

void InputHandler::readButtons() {
    okPressed = digitalRead(okButtonPin) == LOW;
    editPressed = digitalRead(editButtonPin) == LOW;
    backPressed = digitalRead(backButtonPin) == LOW;
    togglePressed = digitalRead(toggleButtonPin) == LOW;
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