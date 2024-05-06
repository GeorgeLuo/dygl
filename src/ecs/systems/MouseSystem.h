#pragma once
#include "ICommand.h"
#include "SelectCommand.h"
#include "EntityManager.h"

class MouseSystem : public System
{

public:
    EntityManager &entityManager;
    ComponentManager &componentManager;
    SceneContext &sceneContext;

    void handleMouseClick(double xpos, double ypos);
    void handleMouseMove(double xpos, double ypos);
    void handleMouseRelease();
    MouseSystem(EntityManager &entityManager, ComponentManager &componentManager, SceneContext &sceneContext);
};

MouseSystem::MouseSystem(EntityManager &entityManager, ComponentManager &componentManager, SceneContext &sceneContext) : entityManager(entityManager), componentManager(componentManager), sceneContext(sceneContext)
{
}

void MouseSystem::handleMouseClick(double xpos, double ypos)
{
    SelectCommand selectCommand(componentManager, xpos, ypos, sceneContext.windowWidth, sceneContext.windowHeight, sceneContext.viewMatrix, sceneContext.projectionMatrix, sceneContext.cameraPosition); // Create the command
    selectCommand.execute();                                                                                                                                                                             // Execute the command
}

void MouseSystem::handleMouseRelease()
{
    DeselectCommand deselectCommand(componentManager);
    deselectCommand.execute();
}

void MouseSystem::handleMouseMove(double xpos, double ypos)
{
    MoveCommand moveCommand(componentManager, xpos, ypos, sceneContext.windowWidth, sceneContext.windowHeight, sceneContext.viewMatrix, sceneContext.projectionMatrix, sceneContext.cameraPosition);
    moveCommand.execute();
}
