#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <Arduino.h>

class InputHandler {
public:
    InputHandler(int okPin, int editPin, int backPin, int togglePin, int rotaryPinA, int rotaryPinB);

    void update();
    bool isOkPressed() const;
    bool isEditPressed() const;
    bool isBackPressed() const;
    bool isToggleButtonPressed() const;
    bool isRotaryIncremented() const;
    bool isRotaryDecremented() const;

private:
    int okButtonPin, editButtonPin, backButtonPin, toggleButtonPin;
    int rotaryPinA, rotaryPinB;
    int lastRotaryState;
    bool okPressed, editPressed, backPressed, togglePressed;
    bool rotaryIncremented, rotaryDecremented;

    void readButtons();
    void readRotaryEncoder();
};

#endif