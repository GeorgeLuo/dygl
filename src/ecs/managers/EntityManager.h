#pragma once

#include "Entity.h"
#include <array>
#include <queue>
#include <cassert>
#include "EventBus.h"
#include <mutex>

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
    std::mutex mutex;

public:
    Entity CreateEntity();

    EntityManager(EventBus &eventBus) : livingEntityCount(0), eventBus(eventBus)
    {
        for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
        {
            availableEntities.push(entity);
        }
    }

    void PublishEntityCreation(Entity entity);

    void DestroyEntity(Entity entity);
};

Entity EntityManager::CreateEntity()
{
    std::lock_guard<std::mutex> lock(mutex);
    assert(!availableEntities.empty() && "Too many entities.");
    Entity id = availableEntities.front();
    availableEntities.pop();
    ++livingEntityCount;

    return id;
}

void EntityManager::PublishEntityCreation(Entity entity)
{
    // eventBus.publish(EntityCreatedEvent(entity));
}

void EntityManager::DestroyEntity(Entity entity)
{
    std::lock_guard<std::mutex> lock(mutex);
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    availableEntities.push(entity);
    --livingEntityCount;

    eventBus.publish(EntityDestroyedEvent(entity));
}