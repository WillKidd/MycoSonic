#include <LCD_I2C.h>

LCD_I2C lcd(0x27, 16, 2); // Default address of most PCF8574 modules, change according


void setup()
{
    lcd.begin();
    lcd.backlight();
    lcd.print("[Filters]");
    lcd.setCursor(12,0);
    lcd.print("Back");
    lcd.setCursor(0,1);
    lcd.print("Volume");
}

void loop()
{
}