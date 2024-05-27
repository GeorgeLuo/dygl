#pragma once
#include "ICommand.h"
#include "ComponentManager.h"
#include "EntityManager.h"

class GenerateTextCommand : public ICommand
{
private:
    ComponentManager &componentManager; // Reference to ComponentManager
    EntityManager &entityManager;
    float r, g, b; // New color values
    float scaleX, scaleY;

public:
    GenerateTextCommand(EntityManager &entityManager, ComponentManager &componentMgr, float scaleX, float scaleY)
        : entityManager(entityManager), componentManager(componentMgr), scaleX(scaleX), scaleY(scaleY) {}

    void execute() override
    {
        
    }
};