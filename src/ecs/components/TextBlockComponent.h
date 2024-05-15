#pragma once
#include <string>

struct TextBlockComponent
{
public:
    std::string blockname;
    std::string textblock;
    TextBlockComponent() = default;
    TextBlockComponent(const std::string blockname) : blockname(blockname) {}
};