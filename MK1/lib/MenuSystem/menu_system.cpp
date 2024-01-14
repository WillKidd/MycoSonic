#include "MENU_SYSTEM.h"
#include <avr/pgmspace.h>

const char strOn[] PROGMEM = "On";
const char strOff[] PROGMEM = "Off";
const char strState[] PROGMEM = "State: ";
const char strValue[] PROGMEM = "Value: ";

// Function to safely copy strings from PROGMEM to a buffer for use
void copyProgMemString(const char *progMemStr, char *buffer, size_t bufferSize)
{
    strncpy_P(buffer, progMemStr, bufferSize);
    buffer[bufferSize - 1] = '\0'; // Ensure null termination
}

MenuItem::MenuItem(const char *name, MenuItemType type)
    : itemName(name), itemType(type), childItemCount(0), childItemCapacity(0), childItems(nullptr) {}

MenuItem::~MenuItem()
{
    for (int i = 0; i < childItemCount; ++i)
    {
        delete childItems[i];
    }
    delete[] childItems;
}

void MenuItem::addChild(MenuItem *child)
{
    if (childItemCount >= childItemCapacity)
    {
        // Increase capacity by 1
        childItemCapacity += 1;
        MenuItem **newChildItems = new MenuItem *[childItemCapacity];

        // Copy existing items
        for (uint8_t i = 0; i < childItemCount; ++i)
        {
            newChildItems[i] = childItems[i];
        }

        // Delete old array and use the new one
        delete[] childItems;
        childItems = newChildItems;
    }

    childItems[childItemCount++] = child;
    child->setParent(this);
}

void MenuItem::setParent(MenuItem *parent)
{
    parentItem = parent;
}

MenuItem *MenuItem::getParent() const
{
    return parentItem;
}

MenuItem **MenuItem::getChildren()
{
    return childItems;
}

uint8_t MenuItem::getChildCount() const
{
    return childItemCount;
}

const char *MenuItem::getName() const
{
    return itemName;
}

MenuItemType MenuItem::getType() const
{
    return itemType;
}

// ToggleMenuItem Implementation
ToggleMenuItem::ToggleMenuItem(const char *name, bool &enabledParam, MenuItemType toggleType)
    : MenuItem(name, toggleType), enabled(enabledParam) {}

void ToggleMenuItem::setEnabled(bool enable)
{
    enabled = enable;
}

bool ToggleMenuItem::isEnabled() const
{
    return enabled;
}

// EditableMenuItem Implementation
EditableMenuItem::EditableMenuItem(const char *name, float &param)
    : MenuItem(name, EDITABLE_ITEM), parameter(param) {}

void EditableMenuItem::setParameterValue(float value)
{
    // Ensure the value is within the specified range
    if (value < 0.0f)
        value = 0.0f;
    else if (value > 99.9f)
        value = 99.9f;
    parameter = value;
}

float EditableMenuItem::getParameterValue() const
{
    return parameter;
}

String EditableMenuItem::getFormattedValue() const
{
    char buffer[5]; // Buffer size for "XX.X" format
    snprintf(buffer, sizeof(buffer), "%4.1f", parameter);
    return String(buffer);
}

EditableMenuItemSingleUint8::EditableMenuItemSingleUint8(const char *name, uint8_t &param)
    : MenuItem(name, EDITABLE_UINT8_ITEM), parameter(param) {}

void EditableMenuItemSingleUint8::setParameterValue(uint8_t value)
{
    parameter = value;
}

uint8_t EditableMenuItemSingleUint8::getParameterValue() const
{
    return parameter;
}

String EditableMenuItemSingleUint8::getFormattedValue() const
{
    char buffer[4];
    snprintf(buffer, sizeof(buffer), "%u", parameter);
    return String(buffer);
}

// MenuHandler Implementation
MenuHandler::MenuHandler(MenuItem *root) : rootItem(root), currentItem(root), currentIndex(0) {}

void MenuHandler::nextItem()
{
    MenuItem *parent = currentItem->getParent();
    if (parent != nullptr)
    {
        uint8_t siblingCount = parent->getChildCount();
        for (uint8_t i = 0; i < siblingCount; ++i)
        {
            if (parent->getChildren()[i] == currentItem)
            {
                currentIndex = (i + 1) % siblingCount;
                currentItem = parent->getChildren()[currentIndex];
                return;
            }
        }
    }
}

void MenuHandler::previousItem()
{
    MenuItem *parent = currentItem->getParent();
    if (parent != nullptr)
    {
        uint8_t siblingCount = parent->getChildCount();
        for (uint8_t i = 0; i < siblingCount; ++i)
        {
            if (parent->getChildren()[i] == currentItem)
            {
                currentIndex = (i - 1 + siblingCount) % siblingCount;
                currentItem = parent->getChildren()[currentIndex];
                return;
            }
        }
    }
}

void MenuHandler::selectItem()
{
    if (currentItem->getChildCount() > 0)
    {
        currentIndex = 0;
        currentItem = currentItem->getChildren()[currentIndex];
    }
}

void MenuHandler::back()
{
    if (currentItem->getParent() != nullptr)
    {
        currentItem = currentItem->getParent();
        currentIndex = 0;
    }
}

MenuItem *MenuHandler::getCurrentItem() const
{
    return currentItem;
}

void MenuHandler::toggleCurrentItem()
{
    MenuItem *item = getCurrentItem();

    if (item->getType() == SINGLE_TOGGLE_ITEM)
    {
        // Logic for SingleToggleMenuItem
        // Disable all toggleable siblings
        for (int i = 0; i < currentItem->getParent()->getChildCount(); ++i)
        {
            MenuItem *sibling = currentItem->getParent()->getChildren()[i];
            if ((sibling->getType() == SINGLE_TOGGLE_ITEM || sibling->getType() == MULTI_TOGGLE_ITEM) && sibling != item)
            {
                static_cast<ToggleMenuItem *>(sibling)->setEnabled(false); // Disable sibling
            }
        }
        static_cast<ToggleMenuItem *>(item)->setEnabled(true); // Enable current item
    }
    else if (item->getType() == MULTI_TOGGLE_ITEM)
    {
        // Logic for MultiToggleMenuItem
        // Check if any SingleToggleMenuItem is enabled among siblings
        bool singleToggleActive = false;
        for (int i = 0; i < currentItem->getParent()->getChildCount(); ++i)
        {
            MenuItem *sibling = currentItem->getParent()->getChildren()[i];
            if (sibling->getType() == SINGLE_TOGGLE_ITEM && static_cast<ToggleMenuItem *>(sibling)->isEnabled())
            {
                singleToggleActive = true;
                break;
            }
        }
        if (!singleToggleActive)
        {
            static_cast<ToggleMenuItem *>(item)->setEnabled(true); // Enable current item if no SingleToggleItem is active
        }
        else
        {
            static_cast<ToggleMenuItem *>(item)->setEnabled(false); // Disable if any SingleToggleItem is active
        }
    }
    // If it's neither, do nothing
}

void MenuHandler::enterEditMode()
{
    if (currentItem->getType() == EDITABLE_ITEM)
    {
        editMode = true;
        originalValue = static_cast<EditableMenuItem *>(currentItem)->getParameterValue();
    }
    else if (currentItem->getType() == EDITABLE_UINT8_ITEM)
    {
        editMode = true;
        originalUint8Value = static_cast<EditableMenuItemSingleUint8 *>(currentItem)->getParameterValue();
    }
}

void MenuHandler::exitEditMode()
{
    editMode = false;
}

bool MenuHandler::isEditMode()
{
    return editMode;
}

void MenuHandler::updateParameterValue(float delta)
{
    if (editMode && currentItem->getType() == EDITABLE_ITEM)
    {
        auto editableItem = static_cast<EditableMenuItem *>(currentItem);
        if ((editableItem->getParameterValue() + delta) >= 0.0)
        {
            editableItem->setParameterValue(editableItem->getParameterValue() + delta);
        }
        else
        {
            editableItem->setParameterValue(editableItem->getParameterValue() + delta);
        }
    }
}

void MenuHandler::saveChanges()
{
    if (editMode)
    {
        exitEditMode();
    }
}

void MenuHandler::discardChanges()
{
    if (editMode)
    {
        if (currentItem->getType() == EDITABLE_ITEM)
        {
            static_cast<EditableMenuItem *>(currentItem)->setParameterValue(originalValue);
        }
        else if (currentItem->getType() == EDITABLE_UINT8_ITEM)
        {
            static_cast<EditableMenuItemSingleUint8 *>(currentItem)->setParameterValue(originalUint8Value);
        }
        exitEditMode();
    }
}

void MenuHandler::setLCDHandler(LCDHandler *lcd)
{
    lcdHandler = lcd;
}

void MenuHandler::updateParameterValueUint8(int8_t delta)
{
    if (editMode && currentItem->getType() == EDITABLE_UINT8_ITEM)
    {
        auto editableItem = static_cast<EditableMenuItemSingleUint8 *>(currentItem);
        int newValue = static_cast<int>(editableItem->getParameterValue()) + delta;
        if (newValue >= 0 && newValue <= UINT8_MAX)
        {
            editableItem->setParameterValue(static_cast<uint8_t>(newValue));
        }
    }
}

void MenuHandler::displayCurrentItem() const
{
    if (!lcdHandler)
        return;                           // Check if LCDHandler is set
    char buffer[16];                      // Buffer for storing strings read from PROGMEM
    memset(buffer, '\0', sizeof(buffer)); // Clear buffer

    // First line: Display the name of the current menu item
    copyProgMemString(currentItem->getName(), buffer, sizeof(buffer));
    Serial.println(buffer);     // Debug print
    lcdHandler->clearDisplay(); // Clear the display before showing new information
    lcdHandler->displayText(buffer, 0, 0);

    // Second line: Additional information based on the item type
    switch (currentItem->getType())
    {
    case SINGLE_TOGGLE_ITEM:
    case MULTI_TOGGLE_ITEM:
    {
        auto toggleItem = static_cast<ToggleMenuItem *>(currentItem);
        copyProgMemString(strState, buffer, sizeof(buffer)); // Copy "State: " into buffer
        lcdHandler->displayText(buffer, 0, 1);               // Display "State: "

        // Update buffer with "On" or "Off" and display
        copyProgMemString(toggleItem->isEnabled() ? strOn : strOff, buffer, sizeof(buffer));
        lcdHandler->displayText(buffer, 7, 1); // Display "On" or "Off"
        break;
    }
    case EDITABLE_ITEM:
    {
        auto editableItem = static_cast<EditableMenuItem *>(currentItem);
        copyProgMemString(strValue, buffer, sizeof(buffer)); // Copy "Value: " into buffer
        lcdHandler->displayText(buffer, 0, 1);               // Display "Value: "

        lcdHandler->displayText(editableItem->getFormattedValue(), 7, 1);
        break;
    }
    case EDITABLE_UINT8_ITEM:
    {
        auto editableUint8Item = static_cast<EditableMenuItemSingleUint8 *>(currentItem);
        copyProgMemString(strValue, buffer, sizeof(buffer));
        lcdHandler->displayText(buffer, 0, 1); // Display "Value: "

        String valueStr = editableUint8Item->getFormattedValue();
        if (valueStr.length() < (16 - 7))
        {
            valueStr += " ";
        }

        lcdHandler->displayText(valueStr, 7, 1);
        break;
    }
    }
}
