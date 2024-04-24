#pragma once
#include <string>

struct EntityCreationMessage {
    int id;
    std::string shape;
    float x, y, z;

    EntityCreationMessage(int id, std::string shape, float x, float y, float z)
        : id(id), shape(shape), x(x), y(y), z(z) {}
};