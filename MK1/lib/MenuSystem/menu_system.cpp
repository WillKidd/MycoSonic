#include "MENU_SYSTEM.h"

MenuItem::MenuItem(const String& name, MenuItemType type) 
    : itemName(name), itemType(type) {}

void MenuItem::addChild(MenuItem* child) {
    if (childItemCount < MAX_MENU_ITEMS) {
        childItems[childItemCount++] = child;
        child->setParent(this);
    }
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

int MenuItem::getChildCount() const {
    return childItemCount;
}

const String& MenuItem::getName() const {
    return itemName;
}

MenuItemType MenuItem::getType() const {
    return itemType;
}

// ToggleMenuItem Implementation
ToggleMenuItem::ToggleMenuItem(const String& name, bool& enabledParam, MenuItemType toggleType)
    : MenuItem(name, toggleType), enabled(enabledParam) {}

void ToggleMenuItem::toggle() {
    enabled = !enabled;
}

bool ToggleMenuItem::isEnabled() const {
    return enabled;
}

// EditableMenuItem Implementation
EditableMenuItem::EditableMenuItem(const String& name, float& param)
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
        int siblingCount = parent->getChildCount();
        for (int i = 0; i < siblingCount; ++i) {
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
        int siblingCount = parent->getChildCount();
        for (int i = 0; i < siblingCount; ++i) {
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
        for (int i = 0; i < currentItem->getParent()->getChildCount(); ++i) {
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
        for (int i = 0; i < currentItem->getParent()->getChildCount(); ++i) {
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
    Serial.println(currentItem->getName());
    lcdHandler->clearDisplay(); // Clear the display before showing new information

    // First line: Display the name of the current menu item
    lcdHandler->displayText(currentItem->getName(), 0, 0);

    // Second line: Additional information based on the item type
     switch(currentItem->getType()) {
        case SINGLE_TOGGLE_ITEM:
        case MULTI_TOGGLE_ITEM: {
            auto toggleItem = static_cast<ToggleMenuItem*>(currentItem);
            String state = toggleItem->isEnabled() ? "On" : "Off";
            lcdHandler->displayText("State: " + state, 0, 1);
            break;
        }
        case EDITABLE_ITEM: {
            auto editableItem = static_cast<EditableMenuItem*>(currentItem);
            lcdHandler->displayText("Value: " + editableItem->getFormattedValue(), 0, 1);
            break;
        }
        case BASE_ITEM:
        break;
         // ... handle other item types as needed ...
    } 
}
