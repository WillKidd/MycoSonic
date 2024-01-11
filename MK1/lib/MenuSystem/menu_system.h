#ifndef MENU_SYSTEM.h
#define MENU_SYSTEM.h

#include <Arduino.h>
#include "LCD_HANDLER.h"

const int MAX_MENU_ITEMS = 6; // Maximum number of child menu items

enum MenuItemType { BASE_ITEM, SINGLE_TOGGLE_ITEM, MULTI_TOGGLE_ITEM, EDITABLE_ITEM };

class MenuItem {
public:
    MenuItem(const String& name, MenuItemType type);

    void addChild(MenuItem* child);
    void setParent(MenuItem* parent);
    MenuItem* getParent() const;
    MenuItem** getChildren();
    int getChildCount() const;
    const String& getName() const;
    MenuItemType getType() const;


private:
    String itemName;
    MenuItemType itemType;
    MenuItem* parentItem;
    MenuItem* childItems[MAX_MENU_ITEMS];
    int childItemCount;
};

class ToggleMenuItem : public MenuItem {
public:
    ToggleMenuItem(const String& name, bool& enabled, MenuItemType toggleType);

    void toggle();
    bool isEnabled() const;

private:
    bool& enabled;
};

class EditableMenuItem : public MenuItem {
public:
    EditableMenuItem(const String& name, float& parameter);

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
    int currentIndex;
    bool editMode;
    float originalValue;
    LCDHandler* lcdHandler; // Pointer to LCDHandler instance
};

#endif
