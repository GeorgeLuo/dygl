#pragma once
#include "Event.h"
#include "Vertex.h"
#include <string>

struct DisplayTextEvent : Event
{
    const std::string text;
    const std::string blockname;
    const float x, y, z;
    bool replace;
    bool focus;
    DisplayTextEvent(const std::string text, const std::string blockname, bool replace, bool focus = true, float x = 0.0f, float y = 0.0f, float z = 0.0f) : text(text), blockname(blockname), replace(replace), focus(focus), x(x), y(y), z(z) {}
};