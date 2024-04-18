#pragma once

#include "Entity.h"
#include <array>
#include <queue>
#include <cassert>

class EntityManager {
private:
    std::queue<Entity> availableEntities{};
    uint32_t livingEntityCount{};

public:
    EntityManager() : livingEntityCount(0) {
        for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
            availableEntities.push(entity);
        }
    }

    Entity CreateEntity() {
        assert(!availableEntities.empty() && "Too many entities.");
        Entity id = availableEntities.front();
        availableEntities.pop();
        ++livingEntityCount;
        return id;
    }

    void DestroyEntity(Entity entity) {
        assert(entity < MAX_ENTITIES && "Entity out of range.");
        availableEntities.push(entity);
        --livingEntityCount;
    }
};
