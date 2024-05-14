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
    EventBus &eventBus;

    void handleMouseClick(double xpos, double ypos);
    void handleMouseMove(double xpos, double ypos);
    void handleMouseRelease();
    MouseSystem(EventBus &eventBus, EntityManager &entityManager, ComponentManager &componentManager, SceneContext &sceneContext);
};

MouseSystem::MouseSystem(EventBus &eventBus, EntityManager &entityManager, ComponentManager &componentManager, SceneContext &sceneContext) : eventBus(eventBus), entityManager(entityManager), componentManager(componentManager), sceneContext(sceneContext)
{
}

void MouseSystem::handleMouseClick(double xpos, double ypos)
{
    SelectCommand selectCommand(componentManager, xpos, ypos, sceneContext.windowWidth, sceneContext.windowHeight, sceneContext.viewMatrix, sceneContext.getPerspectiveProjectionMatrix(), sceneContext.cameraPosition); // Create the command
    selectCommand.execute();                                                                                                                                                                  // Execute the command
}

void MouseSystem::handleMouseRelease()
{
    DeselectCommand deselectCommand(componentManager);
    deselectCommand.execute();
}

void MouseSystem::handleMouseMove(double xpos, double ypos)
{
    MoveCommand moveCommand(eventBus, componentManager, xpos, ypos, sceneContext.windowWidth, sceneContext.windowHeight, sceneContext.viewMatrix, sceneContext.getPerspectiveProjectionMatrix(), sceneContext.cameraPosition);
    moveCommand.execute();
}
