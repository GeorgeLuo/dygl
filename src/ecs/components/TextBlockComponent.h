#pragma once
#include <string>
#include <vector>
#include "TextBlockModification.h"
#include <queue>

struct TextBlockComponent
{
public:
    std::string blockname;
    std::string textblock;
    int texture;
    std::vector<float> color;

    // TODO: maybe a std queue here is fine
    std::queue<TextBlockModification> queuedModifications;

    TextBlockComponent() = default;
    TextBlockComponent(const std::string blockname, int texture = 0, std::vector<float> color = {1.0f, 1.0f, 1.0f, 1.0f}) : blockname(blockname), texture(texture), color(color), queuedModifications()
    {
    }
};