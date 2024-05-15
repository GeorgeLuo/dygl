#pragma once
#include "Event.h"
#include <string>

struct DisplayTextEvent : Event {
    const std::string text;
    DisplayTextEvent(const std::string text) : text(text) {}
};