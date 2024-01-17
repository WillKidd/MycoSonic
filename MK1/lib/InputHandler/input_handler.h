#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <Arduino.h>
#include <Bounce2.h>

class InputHandler {
public:
    InputHandler();
    void update();

    bool isOkPressed() const;
    bool isEditPressed() const;
    bool isBackPressed() const;
    bool isToggleButtonPressed() const;
    bool isUpButtonPressed() const;
    bool isDownButtonPressed() const;

private:
    const uint8_t okButtonPin = 5;
    const uint8_t editButtonPin = 2;
    const uint8_t backButtonPin = 6; 
    const uint8_t toggleButtonPin = 4; 
    const uint8_t upButtonPin = 8;
    const uint8_t downButtonPin = 7;

    // Bounce objects for debouncing
    Bounce okDebouncer, editDebouncer, backDebouncer, toggleDebouncer;
    Bounce upDebouncer, downDebouncer;

    void readButtons();
};

#endif