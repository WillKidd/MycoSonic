#include "INPUT_HANDLER.h"

InputHandler::InputHandler(int okPin, int editPin, int backPin, int togglePin, int upPin, int downPin)
    : okButtonPin(okPin), editButtonPin(editPin), backButtonPin(backPin), toggleButtonPin(togglePin),
      upButtonPin(upPin), downButtonPin(downPin) {

    // Initialize pins with internal pull-up resistors
    pinMode(okButtonPin, INPUT_PULLUP);
    pinMode(editButtonPin, INPUT_PULLUP);
    pinMode(backButtonPin, INPUT_PULLUP);
    pinMode(toggleButtonPin, INPUT_PULLUP);
    pinMode(upButtonPin, INPUT_PULLUP);
    pinMode(downButtonPin, INPUT_PULLUP);

    // Attach pins to debouncers and set debounce interval
    okDebouncer.attach(okButtonPin);
    okDebouncer.interval(25);
    editDebouncer.attach(editButtonPin);
    editDebouncer.interval(25);
    backDebouncer.attach(backButtonPin);
    backDebouncer.interval(25);
    toggleDebouncer.attach(toggleButtonPin);
    toggleDebouncer.interval(25);
    upDebouncer.attach(upButtonPin);
    upDebouncer.interval(25);
    downDebouncer.attach(downButtonPin);
    downDebouncer.interval(25);
}

void InputHandler::update() {
    readButtons();
}

void InputHandler::readButtons() {
    okDebouncer.update();
    editDebouncer.update();
    backDebouncer.update();
    toggleDebouncer.update();
    upDebouncer.update();
    downDebouncer.update();
}

bool InputHandler::isOkPressed() const {
    return okDebouncer.fell();
}

bool InputHandler::isEditPressed() const {
    return editDebouncer.fell();
}

bool InputHandler::isBackPressed() const {
    return backDebouncer.fell();
}

bool InputHandler::isToggleButtonPressed() const {
    return toggleDebouncer.fell();
}

bool InputHandler::isUpButtonPressed() const {
    return upDebouncer.fell();
}

bool InputHandler::isDownButtonPressed() const {
    return downDebouncer.fell();
}
