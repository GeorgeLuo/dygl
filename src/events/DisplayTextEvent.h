#pragma once
#include "Event.h"
#include "Vertex.h"
#include <string>

struct DisplayTextEvent : Event {
    const std::string text;
    const std::string blockname;
    const float x, y, z;
    DisplayTextEvent(const std::string text, const std::string blockname, float x, float y, float z) : text(text), blockname(blockname), x(x), y(y), z(z) {}
};