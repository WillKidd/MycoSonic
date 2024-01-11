#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <Arduino.h>

class InputHandler {
public:
    InputHandler(int okPin, int editPin, int backPin, int togglePin, int upPin, int downPin);

    void update();
    bool isOkPressed() const;
    bool isEditPressed() const;
    bool isBackPressed() const;
    bool isToggleButtonPressed() const;
    bool isUpButtonPressed() const;
    bool isDownButtonPressed() const;

private:
    int okButtonPin, editButtonPin, backButtonPin, toggleButtonPin;
    int upButtonPin, downButtonPin;
    bool okPressed, editPressed, backPressed, togglePressed;
    bool upPressed, downPressed;

    void readButtons();
};

#endif