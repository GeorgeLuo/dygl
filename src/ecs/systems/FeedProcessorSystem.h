#pragma once
#include "System.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "TextBlockComponent.h"
#include "SystemLogger.h"
#include "TransformComponent.h"
#include "Text.h"
#include "BoundingBoxComponent.h"

/**
 * The logging system maps the system logger to renderable entities
 */

// TODO: use a map here (if this gets used often), also move somewhere more appropriate
Entity GetEntityByBlockname(ComponentManager &componentManager, const std::string blockname)
{
    auto entities = componentManager.GetEntitiesWithComponent<TextBlockComponent>();

    // find the block by blockname
    for (auto entity : entities)
    {
        TextBlockComponent &component = componentManager.GetComponent<TextBlockComponent>(entity);
        if (component.blockname == blockname)
        {
            return entity;
        }
    }
    return INVALID_ENTITY;
}

enum LogLevel
{

};

class FeedProcessorSystem : public System
{
public:
    FeedProcessorSystem(EntityManager &entityManager, ComponentManager &componentManager, SystemLogger *logger);
    void Update(float deltaTime) override;

private:
    EntityManager &entityManager;
    ComponentManager &componentManager;
};

FeedProcessorSystem::FeedProcessorSystem(EntityManager &entityManager, ComponentManager &componentManager, SystemLogger *logger) : System(logger), entityManager(entityManager), componentManager(componentManager)
{
}

void FeedProcessorSystem::Update(float deltaTime)
{
    Loggable loggable;
    while (logger->TryPop(loggable))
    {
        // push change to logging entities
        std::string blockname = loggable.blockname;

        auto entity = GetEntityByBlockname(componentManager, blockname);
        if (GetEntityByBlockname(componentManager, blockname) == INVALID_ENTITY)
        {
            // create the entity to contain the text block
            entity = entityManager.CreateEntity();
            componentManager.AddComponent(entity, TextBlockComponent(blockname));
            // componentManager.AddComponent(entity, TransformComponent(-2.6f, -1.9f, 0.0f, scale, scale));
            // componentManager.AddComponent(entity, TransformComponent(-2.6f, -1.9f, 0.0f, 0.004f, 0.004f));
            componentManager.AddComponent(entity, TransformComponent(-2.6f, -1.9f, 0.0f));
            componentManager.AddComponent(entity, BoundingBoxComponent(-2.6f, -1.9f, 3000, 1000));
            entityManager.PublishEntityCreation(entity);
        }

        auto &component = componentManager.GetComponent<TextBlockComponent>(entity);
        component.queuedModifications.push(TextBlockModification(REPLACE, loggable.content));
    }
}