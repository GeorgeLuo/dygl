#pragma once
#include "Event.h"
#include <string>

enum KeyboardEntryType
{
    CHARACTER,
    DELETE,
    ENTER,
    PASTE
};

struct KeyboardEntryEvent : public Event
{
public:
    KeyboardEntryType entryType;
    std::string text;
    KeyboardEntryEvent(KeyboardEntryType entryType, std::string text) : entryType(entryType), text(text){};
};
