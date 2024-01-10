#include "LCD_HANDLER.H"

LCDHandler::LCDHandler(uint8_t lcdAddr, uint8_t lcdCols, uint8_t lcdRows)
    : lcd(lcdAddr, lcdCols, lcdRows) {}

void LCDHandler::init() {
    lcd.begin();
    lcd.backlight();
}

void LCDHandler::clearDisplay() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastDisplayUpdate >= displayUpdateInterval) {
        lcd.clear();
    }
}

void LCDHandler::displayText(const String& text, uint8_t col, uint8_t row) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastDisplayUpdate >= displayUpdateInterval) {
        lcd.setCursor(col, row);
        lcd.print(text);
        lastDisplayUpdate = currentMillis; // Update the last display time
    }
}
