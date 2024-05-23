#pragma once
#include <string>

enum TextBlockModificationType
{
    CHARACTER,
    DELETE,
    ENTER,
    SPACEBAR,
    PASTE,
    REPLACE
};

struct TextBlockModification
{
public:
    TextBlockModificationType entryType;
    std::string text;
    TextBlockModification(TextBlockModificationType entryType, std::string text) : entryType(entryType), text(text){};
    TextBlockModification() = default;
};
