#ifndef MENU_SYSTEM.h
#define MENU_SYSTEM.h

#include <Arduino.h>
#include "LCD_HANDLER.h"

const uint8_t MAX_MENU_ITEMS = 6; // Maximum number of child menu items

enum MenuItemType { BASE_ITEM, SINGLE_TOGGLE_ITEM, MULTI_TOGGLE_ITEM, EDITABLE_ITEM };
class MenuItem {
public:
    MenuItem(const char* name, MenuItemType type);
    ~MenuItem();

    void addChild(MenuItem* child);
    void setParent(MenuItem* parent);
    MenuItem* getParent() const;
    MenuItem** getChildren();
    uint8_t getChildCount() const;
    const char* getName() const;
    MenuItemType getType() const;


private:
    const char* itemName;
    MenuItemType itemType;
    MenuItem* parentItem;
    MenuItem** childItems;
    uint8_t childItemCount;
    uint8_t childItemCapacity;
};

class ToggleMenuItem : public MenuItem {
public:
    ToggleMenuItem(const char* name, bool& enabled, MenuItemType toggleType);

    void toggle();
    bool isEnabled() const;
    MenuItem* childItems[0];

private:
    bool& enabled;
};

class EditableMenuItem : public MenuItem {
public:
    EditableMenuItem(const char* name, float& parameter);

    void setParameterValue(float value);
    float getParameterValue() const;
    String getFormattedValue() const;


private:
    float& parameter;
    MenuItem* childItems[0];
};

class MenuHandler {
public:
    MenuHandler(MenuItem* root);

    void nextItem();
    void previousItem();
    void selectItem();
    void back();

    MenuItem* getCurrentItem() const;

    void toggleCurrentItem();

    void enterEditMode();
    void exitEditMode();
    bool isEditMode();
    void updateParameterValue(float delta);
    void saveChanges();
    void discardChanges();

    void setLCDHandler(LCDHandler* lcd); // Method to set the LCDHandler
    void displayCurrentItem() const; // Method to display the current menu item using LCD


private:
    MenuItem* rootItem;
    MenuItem* currentItem;
    uint8_t currentIndex;
    bool editMode;
    float originalValue;
    LCDHandler* lcdHandler; // Pointer to LCDHandler instance
};

#endif
