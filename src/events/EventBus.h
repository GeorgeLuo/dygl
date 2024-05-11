#pragma once
#include <iostream>
#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <typeinfo>
#include <execinfo.h> // For backtrace (POSIX only)
#include <iostream>
#include <unistd.h> // For STDERR_FILENO
#include "Event.h"

// Event bus class
class EventBus
{
public:
    EventBus()
    {
        std::cout << "EventBus created and ready to handle events." << std::endl;
        // For example, pre-reserving space for known events could go here
        // handlers.reserve(10); // Pre-reserve space for performance optimization
    }

    template <typename EventType>
    void subscribe(std::function<void(const EventType &)> handler)
    {
        handlers[typeid(EventType)].push_back([=](const Event &event)
                                              { handler(static_cast<const EventType &>(event)); });
    }

    template <typename EventType>
    void publish(const EventType &event)
    {
        std::type_index typeIndex = typeid(EventType);
        if (handlers.find(typeIndex) != handlers.end())
        {
            for (auto &handler : handlers[typeIndex])
            {
                handler(event); // Invoke all handlers for this event type
            }
        }
    }

private:
    // Maps type index to a list of event handlers (type-erased)
    std::unordered_map<std::type_index, std::vector<std::function<void(const Event &)>>> handlers;
};