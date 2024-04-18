#pragma once
#include <tuple>
#include <vector>
#include "ConcurrentQueue.h"
#include <string>

// Structure to hold initial entity creation data
struct EntityCreationMessage {
    int id; // Unique identifier for the entity (could be an Entity ID if applicable)
    std::string shape;
    float x, y, z; // Position data

    EntityCreationMessage(int id, std::string shape, float x, float y, float z)
        : id(id), shape(shape), x(x), y(y), z(z) {}
};

// New structure for holding entity deletion data
struct EntityDeletionMessage {
    int id; // Unique identifier for the entity to be deleted

    EntityDeletionMessage(int id) : id(id) {}
};

struct QueueCollection {
    ConcurrentQueue<std::tuple<float, float, float>> colorQueue;
    ConcurrentQueue<std::tuple<float, float, float>> positionQueue;
    ConcurrentQueue<std::vector<EntityCreationMessage>> entityCreationQueue; // Queue for batch entity creation messages
    ConcurrentQueue<std::vector<EntityDeletionMessage>> entityDeletionQueue; // Queue for batch entity deletion messages
};