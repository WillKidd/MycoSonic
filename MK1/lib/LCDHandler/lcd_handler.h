#ifndef LCD_HANDLER_H
#define LCD_HANDLER_H

#include <Arduino.h>
#include <LCD_I2C.h>

class LCDHandler {
public:
    LCDHandler(uint8_t lcdAddr, uint8_t lcdCols, uint8_t lcdRows);

    void init();
    void clearDisplay();
    void displayText(const String& text, uint8_t col, uint8_t row);

private:
    unsigned long lastDisplayUpdate = 0; // Last time the display was updated
    const unsigned long displayUpdateInterval = 100; // Display update interval in milliseconds
    LCD_I2C lcd;
    uint8_t cols, rows;
};

#endif