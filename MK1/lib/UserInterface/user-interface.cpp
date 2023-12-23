#include <SPI.h>
#include <LCD_I2C.h>
#include <Wire.h>

LCD_I2C lcd(0x27, 16, 2);



struct MenuItem {
    String name;
    MenuItem* parent;
    MenuItem* children[10];
    int childrenCount=0;
    int value;  // Current value for adjustable setting
    int minValue;
    int maxValue;
    int step;   // Increment/decrement step
    void (*action)(); // Function pointer for actions
};

// Define global variables and menu items
MenuItem* currentMenu;
bool inAdjustmentMode = false;

// Initialize LCD and other components
// ... (LCD initialization code)

void navigateMenu(int direction) {
    // Logic to move up or down the menu list
    // ...
}

void selectMenuItem() {
    if (currentMenu->childrenCount > 0) {
        // Navigate to submenu
    } else {
        // Enter value adjustment mode
        inAdjustmentMode = true;
    }
}

void adjustValue(int direction) {
    if (inAdjustmentMode) {
        currentMenu->value += direction * currentMenu->step;
        currentMenu->value = max(currentMenu->value, currentMenu->minValue);
        currentMenu->value = min(currentMenu->value, currentMenu->maxValue);
    }
}

void setup() {
    // Initialize LCD and set up inputs
    // ...
}

void loop() {
    // Check for input
    if (inAdjustmentMode) {
        // Adjust the value of the current menu item
        // ...
    } else {
        // Navigate through the menu
        // ...
    }

    // Update the display based on the current menu item and its value
    // ...
}

// Additional functions for menu handling and actions
// ...
