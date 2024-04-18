#pragma once

#include <vector>

struct TagComponent
{
    std::vector<std::string> tags;
    TagComponent() = default;

    // Example methods to interact with tags
    void AddTag(const std::string &tag)
    {
        tags.push_back(tag);
    }

    bool HasTag(const std::string &tag) const
    {
        return std::find(tags.begin(), tags.end(), tag) != tags.end();
    }

    void RemoveTag(const std::string &tag)
    {
        auto it = std::find(tags.begin(), tags.end(), tag);
        if (it != tags.end())
        {
            tags.erase(it);
        }
    }
};