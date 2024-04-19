#pragma once

#include "Entity.h"
#include <array>
#include <queue>
#include <cassert>
#include "EventBus.h"

struct EntityCreatedEvent : Event
{
    Entity entity;

    EntityCreatedEvent(Entity entity) : entity(entity) {}
};

struct EntityDestroyedEvent : Event
{
    Entity entity;

    EntityDestroyedEvent(Entity entity) : entity(entity) {}
};

class EntityManager
{
private:
    std::queue<Entity> availableEntities{};
    uint32_t livingEntityCount{};
    EventBus &eventBus;

public:
    EntityManager(EventBus &eventBus) : livingEntityCount(0), eventBus(eventBus)
    {
        for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
        {
            availableEntities.push(entity);
        }
    }

    Entity CreateEntity()
    {
        assert(!availableEntities.empty() && "Too many entities.");
        Entity id = availableEntities.front();
        availableEntities.pop();
        ++livingEntityCount;

        eventBus.publish(EntityCreatedEvent(id));
        return id;
    }

    void DestroyEntity(Entity entity)
    {
        assert(entity < MAX_ENTITIES && "Entity out of range.");
        availableEntities.push(entity);
        --livingEntityCount;

        eventBus.publish(EntityDestroyedEvent(entity));
    }
};
