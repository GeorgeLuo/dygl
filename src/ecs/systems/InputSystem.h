#pragma once
#include "ICommand.h"
#include "SelectCommand.h"
#include "EntityManager.h"

class InputSystem
{

public:
    EntityManager &entityManager;
    ComponentManager &componentManager;

    void handleMouseClick(double xpos, double ypos);
    void handleMouseMove(double xpos, double ypos);
    void handleMouseRelease();
    InputSystem(EntityManager &entityManager, ComponentManager &componentManager): entityManager(entityManager), componentManager(componentManager)
    {
    }
};

void InputSystem::handleMouseClick(double xpos, double ypos)
{
    SelectCommand selectCommand(componentManager, xpos, ypos); // Create the command
    selectCommand.execute();                                   // Execute the command
}

void InputSystem::handleMouseMove(double xpos, double ypos)
{
    // Generate a command for dragging an entity
}

void InputSystem::handleMouseRelease()
{
    // Generate a command for releasing the entity
}
