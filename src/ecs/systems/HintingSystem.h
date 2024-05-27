#pragma once

#include "System.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "GameStateComponent.h"
#include "GenerateTextCommand.h"

/**
 * The hinting system is used to display helpful messages based on the current mode
 */

class HintingSystem : public System
{
public:
    void Update(float deltaTime) override;
    HintingSystem::HintingSystem(EntityManager &entityManager, ComponentManager &componentManager) : entityManager(entityManager), componentManager(componentManager){};

private:
    EntityManager &entityManager;
    ComponentManager &componentManager;
};

void HintingSystem::Update(float deltaTime)
{
    auto gameState = componentManager.GetComponent<GameStateComponent>(componentManager.GetEntityWithComponent<GameStateComponent>());
    if (gameState.gameMode == ENTITY_MANAGEMENT)
    {
        // display instruction create entity: shift + click to generate shape in "clipboard" (last shape/default shape)
        GenerateTextCommand command(entityManager, componentManager, 0.0f, 0.0f);
        command.execute();
    }
}