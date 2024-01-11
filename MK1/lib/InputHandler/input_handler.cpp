#include "INPUT_HANDLER.h"

InputHandler::InputHandler(int okPin, int editPin, int backPin, int togglePin, int upPin, int downPin)
    : okButtonPin(okPin), editButtonPin(editPin), backButtonPin(backPin), toggleButtonPin(togglePin),
      upButtonPin(upPin), downButtonPin(downPin), 
      okPressed(false), editPressed(false),
      backPressed(false), togglePressed(false), 
      upPressed(false), downPressed(false) {

    pinMode(okButtonPin, INPUT);
    pinMode(editButtonPin, INPUT);
    pinMode(backButtonPin, INPUT);
    pinMode(toggleButtonPin, INPUT);
    pinMode(upPin, INPUT);
    pinMode(downPin, INPUT);
}

void InputHandler::update() {
    readButtons();
}

void InputHandler::readButtons() {
    okPressed = digitalRead(okButtonPin) == HIGH;
    editPressed = digitalRead(editButtonPin) == HIGH;
    backPressed = digitalRead(backButtonPin) == HIGH;
    togglePressed = digitalRead(toggleButtonPin) == HIGH;
    upPressed = digitalRead(upButtonPin) == HIGH;
    downPressed = digitalRead(downButtonPin) == HIGH;
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

bool InputHandler::isUpButtonPressed() const {
    return upPressed;
}

bool InputHandler::isDownButtonPressed() const {
    return downPressed;
}