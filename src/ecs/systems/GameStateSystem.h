#pragma once
#include "System.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "GameStateComponent.h"

class GameStateSystem : public System
{
public:
    GameStateSystem(EntityManager &entityManager, ComponentManager &componentManager);
    void Initialize();

private:
    ComponentManager &componentManager;
    EntityManager &entityManager;
};

GameStateSystem::GameStateSystem(EntityManager &entityManager, ComponentManager &componentManager) : entityManager(entityManager), componentManager(componentManager)
{
}

void GameStateSystem::Initialize()
{
    auto entity = entityManager.CreateEntity();
    componentManager.AddComponent(entity, GameStateComponent(FREE_TYPE));
}
