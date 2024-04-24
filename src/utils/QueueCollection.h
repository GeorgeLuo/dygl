#pragma once
#include <tuple>
#include <vector>
#include "ConcurrentQueue.h"
#include "EntityCreationMessage.h"
#include "EntityCreationMessageV2.h"
#include "EntityDeletionMessage.h"
#include <string>

struct QueueCollection {
    ConcurrentQueue<std::tuple<float, float, float>> colorQueue;
    ConcurrentQueue<std::tuple<float, float, float>> positionQueue;
    ConcurrentQueue<std::vector<EntityCreationMessage>> entityCreationQueue; // Queue for batch entity creation messages
    ConcurrentQueue<std::vector<EntityCreationMessageV2>> entityCreationV2Queue; // Queue for batch entity creation messages
    ConcurrentQueue<std::vector<EntityDeletionMessage>> entityDeletionQueue; // Queue for batch entity deletion messages
};