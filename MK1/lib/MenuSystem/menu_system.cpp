#include "MENU_SYSTEM.h"
#include <avr/pgmspace.h>

const char strOn[] PROGMEM = "On";
const char strOff[] PROGMEM = "Off";
const char strState[] PROGMEM = "State: ";
const char strValue[] PROGMEM = "Value: ";

// Function to safely copy strings from PROGMEM to a buffer for use
void copyProgMemString(const char* progMemStr, char* buffer, size_t bufferSize) {
    strncpy_P(buffer, progMemStr, bufferSize);
    buffer[bufferSize - 1] = '\0'; // Ensure null termination
}

MenuItem::MenuItem(const char* name, MenuItemType type) 
    : itemName(name), itemType(type), childItemCount(0), childItemCapacity(0), childItems(nullptr) {}

MenuItem::~MenuItem() {
    for (int i = 0; i < childItemCount; ++i) {
        delete childItems[i];
    }
    delete[] childItems;
}

void MenuItem::addChild(MenuItem* child) {
    if (childItemCount >= childItemCapacity) {
        // Increase capacity by 1
        childItemCapacity += 1;
        MenuItem** newChildItems = new MenuItem*[childItemCapacity];

        // Copy existing items
        for (uint8_t i = 0; i < childItemCount; ++i) {
            newChildItems[i] = childItems[i];
        }

        // Delete old array and use the new one
        delete[] childItems;
        childItems = newChildItems;
    }

    childItems[childItemCount++] = child;
    child->setParent(this);
}


void MenuItem::setParent(MenuItem* parent) {
    parentItem = parent;
}

MenuItem* MenuItem::getParent() const {
    return parentItem;
}

MenuItem** MenuItem::getChildren() {
    return childItems;
}

uint8_t MenuItem::getChildCount() const {
    return childItemCount;
}

const char* MenuItem::getName() const {
    return itemName;
}

MenuItemType MenuItem::getType() const {
    return itemType;
}

// ToggleMenuItem Implementation
ToggleMenuItem::ToggleMenuItem(const char* name, bool& enabledParam, MenuItemType toggleType)
    : MenuItem(name, toggleType), enabled(enabledParam) {}

void ToggleMenuItem::toggle() {
    enabled = !enabled;
}

bool ToggleMenuItem::isEnabled() const {
    return enabled;
}

// EditableMenuItem Implementation
EditableMenuItem::EditableMenuItem(const char* name, float& param)
    : MenuItem(name, EDITABLE_ITEM), parameter(param) {}

void EditableMenuItem::setParameterValue(float value) {
    parameter = value;
}

float EditableMenuItem::getParameterValue() const {
    return parameter;
}

String EditableMenuItem::getFormattedValue() const {
    char buffer[10]; // Buffer to hold the formatted string
    dtostrf(parameter, 4, 2, buffer); // Convert float to string with 2 decimal places
    return String(buffer);
}

// MenuHandler Implementation
MenuHandler::MenuHandler(MenuItem* root) : rootItem(root), currentItem(root), currentIndex(0) {}

void MenuHandler::nextItem() {
    MenuItem* parent = currentItem->getParent();
    if (parent != nullptr) {
        uint8_t siblingCount = parent->getChildCount();
        for (uint8_t i = 0; i < siblingCount; ++i) {
            if (parent->getChildren()[i] == currentItem) {
                currentIndex = (i + 1) % siblingCount;
                currentItem = parent->getChildren()[currentIndex];
                return;
            }
        }
    }
}


void MenuHandler::previousItem() {
    MenuItem* parent = currentItem->getParent();
    if (parent != nullptr) {
        uint8_t siblingCount = parent->getChildCount();
        for (uint8_t i = 0; i < siblingCount; ++i) {
            if (parent->getChildren()[i] == currentItem) {
                currentIndex = (i - 1 + siblingCount) % siblingCount;
                currentItem = parent->getChildren()[currentIndex];
                return;
            }
        }
    }
}


void MenuHandler::selectItem() {
    if (currentItem->getChildCount() > 0) {
        currentIndex = 0;
        currentItem = currentItem->getChildren()[currentIndex];
    }
}

void MenuHandler::back() {
    if (currentItem->getParent() != nullptr) {
        currentItem = currentItem->getParent();
        currentIndex = 0;
    }
}

MenuItem* MenuHandler::getCurrentItem() const {
    return currentItem;
}

void MenuHandler::toggleCurrentItem() {
    MenuItem* item = getCurrentItem();

    if (item->getType() == SINGLE_TOGGLE_ITEM) {
        // Logic for SingleToggleMenuItem
        // Disable all toggleable siblings
        for (uint8_t i = 0; i < currentItem->getParent()->getChildCount(); ++i) {
            MenuItem* sibling = currentItem->getParent()->getChildren()[i];
            if ((sibling->getType() == SINGLE_TOGGLE_ITEM || sibling->getType() == MULTI_TOGGLE_ITEM) && sibling != item) {
                static_cast<ToggleMenuItem*>(sibling)->toggle(); // Disable sibling
            }
        }
        static_cast<ToggleMenuItem*>(item)->toggle(); // Toggle current item
    } else if (item->getType() == MULTI_TOGGLE_ITEM) {
        // Logic for MultiToggleMenuItem
        // Ensure no SingleToggleMenuItem is enabled among siblings
        bool singleToggleActive = false;
        for (uint8_t i = 0; i < currentItem->getParent()->getChildCount(); ++i) {
            MenuItem* sibling = currentItem->getParent()->getChildren()[i];
            if (sibling->getType() == SINGLE_TOGGLE_ITEM && static_cast<ToggleMenuItem*>(sibling)->isEnabled()) {
                singleToggleActive = true;
                break;
            }
        }
        if (!singleToggleActive) {
            static_cast<ToggleMenuItem*>(item)->toggle(); // Toggle current item
        }
    }
    // If it's neither, do nothing
}

void MenuHandler::enterEditMode() {
    if (currentItem->getType() == EDITABLE_ITEM) {
        editMode = true;
        originalValue = static_cast<EditableMenuItem*>(currentItem)->getParameterValue();
    }
}

void MenuHandler::exitEditMode() {
    editMode = false;
}

bool MenuHandler::isEditMode(){
    return editMode;
}

void MenuHandler::updateParameterValue(float delta) {
    if (editMode && currentItem->getType() == EDITABLE_ITEM) {
        auto editableItem = static_cast<EditableMenuItem*>(currentItem);
        if ((editableItem->getParameterValue() + delta) >= 0.0){
            editableItem->setParameterValue(editableItem->getParameterValue() + delta);
        }
        else {
            editableItem->setParameterValue(editableItem->getParameterValue() + delta);
        }
        
    }
}

void MenuHandler::saveChanges() {
    if (editMode && currentItem->getType() == EDITABLE_ITEM) {
        exitEditMode(); // Saves changes and exits edit mode
    }
}

void MenuHandler::discardChanges() {
    if (editMode && currentItem->getType() == EDITABLE_ITEM) {
        static_cast<EditableMenuItem*>(currentItem)->setParameterValue(originalValue);
        exitEditMode(); // Discards changes and exits edit mode
    }
}

void MenuHandler::setLCDHandler(LCDHandler* lcd) {
    lcdHandler = lcd;
}

void MenuHandler::displayCurrentItem() const {
    if (!lcdHandler) return; // Check if LCDHandler is set
    char buffer[16]; // Buffer for storing strings read from PROGMEM

    // First line: Display the name of the current menu item
    // Assuming getName() correctly handles PROGMEM
    copyProgMemString(currentItem->getName(), buffer, sizeof(buffer));
    Serial.println(buffer);  // Debug print
    lcdHandler->clearDisplay(); // Clear the display before showing new information
    lcdHandler->displayText(buffer, 0, 0);

    // Second line: Additional information based on the item type
    switch(currentItem->getType()) {
        case SINGLE_TOGGLE_ITEM:
        case MULTI_TOGGLE_ITEM: {
            auto toggleItem = static_cast<ToggleMenuItem*>(currentItem);
            copyProgMemString(strState, buffer, sizeof(buffer)); // Copy "State: " into buffer
            lcdHandler->displayText(buffer, 0, 1); // Display "State: "

            // Update buffer with "On" or "Off" and display
            copyProgMemString(toggleItem->isEnabled() ? strOn : strOff, buffer, sizeof(buffer));
            lcdHandler->displayText(buffer, 7, 1); // Display "On" or "Off"
            break;
        }
        case EDITABLE_ITEM: {
            auto editableItem = static_cast<EditableMenuItem*>(currentItem);
            copyProgMemString(strValue, buffer, sizeof(buffer)); // Copy "Value: " into buffer
            lcdHandler->displayText(buffer, 0, 1); // Display "Value: "

            // Assuming getFormattedValue() returns a RAM-based string
            lcdHandler->displayText(editableItem->getFormattedValue(), 7, 1);
            break;
        }
        case BASE_ITEM:
        break;
        // ... handle other item types as needed ...
    }
}

