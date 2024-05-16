#pragma once
#include <System.h>
#include "Text.h"
#include "DisplayTextEvent.h"
#include "KeyboardEntryEvent.h"

enum KeyboardEntryMode
{
    FREE_TYPE,
    ENTITY_MANAGEMENT,
    SCENE_NAVIGATION
};

class KeyboardInputSystem : public System
{
public:
    EventBus &eventBus;

    KeyboardInputSystem(EventBus &eventBus);
    void InputChar(KeyboardEntryType entryType, int character, bool shiftPressed, bool ctrlPressed, bool altPressed);
    void InputChar(KeyboardEntryType character);

private:
    KeyboardEntryMode mode;
};

KeyboardInputSystem::KeyboardInputSystem(EventBus &eventBus) : eventBus(eventBus) {}

void KeyboardInputSystem::InputChar(KeyboardEntryType entryType, int character, bool shiftPressed, bool ctrlPressed, bool altPressed)
{
    std::string c;
    if (entryType == CHARACTER)
        c = getChar(character);

    // determine if it's a meta command
    if (ctrlPressed && shiftPressed)
    {
        if (c == "e")
        {
            mode = ENTITY_MANAGEMENT;
            // tell text overlay system to display we're in meta mode, entity management
            return;
        }
        else if (c == "t")
        {
            mode = SCENE_NAVIGATION;
            // tell text overlay system we're in travel mode
            return;
        }
        else if (c == "f")
        {
            mode = FREE_TYPE;
            return;
        }
    }

    if (mode == FREE_TYPE)
    {
        // enter means lock text block from appends 
        // TODO: should probably handle this at the top of method
        if (c == "\n")
        {
            entryType = ENTER;
        }

        std::string realC = shiftPressed ? getShiftedChar(character) : getChar(character);
        // eventBus.publish(DisplayTextEvent(realC, "free_type", false, true, 0.0f, 0.0f, 0.0f));
        eventBus.publish(KeyboardEntryEvent(entryType, realC));
    }
}
