#pragma once
#include "Event.h"
#include <string>

enum TextBlockModificationType
{
    CHARACTER,
    DELETE,
    ENTER,
    PASTE,
    SPACEBAR
};

struct TextBlockModification : public Event
{
public:
    TextBlockModificationType entryType;
    std::string text;
    TextBlockModification(TextBlockModificationType entryType, std::string text) : entryType(entryType), text(text){};
    TextBlockModification() = default;
};
