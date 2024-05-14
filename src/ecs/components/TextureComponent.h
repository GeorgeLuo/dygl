#pragma once
#include <vector>

struct TextureComponent
{
    std::vector<int> textureIDs;

    TextureComponent() = default;
    TextureComponent(unsigned int id) { textureIDs.push_back(id); }
};
