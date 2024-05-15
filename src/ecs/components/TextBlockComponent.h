#pragma once
#include <string>
#include <vector>

struct TextBlockComponent
{
public:
    std::string blockname;
    std::string textblock;
    int texture;
    std::vector<float> color;

    TextBlockComponent() = default;
    TextBlockComponent(const std::string blockname, int texture = 0, std::vector<float> color = {1.0f, 1.0f, 1.0f, 1.0f}) : blockname(blockname), texture(texture), color(color) {}
};