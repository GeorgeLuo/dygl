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

// Event base class
struct Event
{
    virtual ~Event() = default;
};

// A specific event type
struct MyEvent : public Event
{
    int data;
};

// Event bus class
class EventBus
{
public:
    EventBus()
    {
        // Perform any required initialization here
        std::cout << "EventBus created and ready to handle events." << std::endl;
        // For example, pre-reserving space for known events could go here
        // handlers.reserve(10); // Pre-reserve space for performance optimization
    }

    template <typename EventType>
    void subscribe(std::function<void(const EventType &)> handler)
    {
        // void *array[10];
        // size_t size;

        // get void*'s for all entries on the stack
        // size = backtrace(array, 10);

        // print out all the frames to stderr
        // fprintf(stderr, "Trace of subscribe call:\n");
        // backtrace_symbols_fd(array, size, STDERR_FILENO);

        handlers[typeid(EventType)].push_back([=](const Event &event)
                                              { handler(static_cast<const EventType &>(event)); });
        // std::cout << "Subscribed to event type: " << typeid(EventType).name() << std::endl;
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