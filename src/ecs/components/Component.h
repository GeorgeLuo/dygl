#pragma once
#include <cstdint>
#include <array>

// ComponentType is used as an identifier for component types
using ComponentType = uint8_t;
const ComponentType MAX_COMPONENTS = 32;

class Component {
public:
    virtual ~Component() = default;
};
