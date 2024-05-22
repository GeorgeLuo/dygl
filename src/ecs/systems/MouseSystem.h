#pragma once
#include "ICommand.h"
#include "MouseCommands.h"
#include "EntityManager.h"

enum MouseActionType
{
    LEFT_PRESS,
    RIGHT_PRESS,
    MOVE,
    LEFT_RELEASE,
    RIGHT_RELEASE,
};

class MouseAction
{
public:
    float xpos, ypos;
    MouseActionType mouseActionType;

    MouseAction() = default;
    MouseAction(MouseActionType mouseActionType, float xpos, float ypos) : mouseActionType(mouseActionType), xpos(xpos), ypos(ypos){};
};

class MouseSystem : public System
{

public:
    EntityManager &entityManager;
    ComponentManager &componentManager;

    SceneContext &sceneContext;

    void Update(float deltaTime) override;

    void LeftPress(double xpos, double ypos, bool shiftPressed = false, bool altPressed = false, bool ctrlPressed = false);
    void LeftRelease(double xpos, double ypos);
    void Move(double xpos, double ypos);

    MouseSystem(EntityManager &entityManager, ComponentManager &componentManager, SceneContext &sceneContext);

private:
    ConcurrentQueue<MouseAction> mouseActionQueue;

    // queueing strategy
    void handleLeftPress(double xpos, double ypos);
    void handleMouseMove(double xpos, double ypos);
    void handleLeftRelease(double xpos, double ypos);
};

MouseSystem::MouseSystem(EntityManager &entityManager, ComponentManager &componentManager, SceneContext &sceneContext) : entityManager(entityManager), componentManager(componentManager), sceneContext(sceneContext)
{
}

void MouseSystem::Update(float deltaTime)
{
    MouseAction mouseAction;
    while (mouseActionQueue.TryPop(mouseAction))
    {
        if (mouseAction.mouseActionType == LEFT_PRESS)
        {
            handleLeftPress(mouseAction.xpos, mouseAction.ypos);
        }
        else if (mouseAction.mouseActionType == MOVE)
        {
            handleMouseMove(mouseAction.xpos, mouseAction.ypos);
        }
        else if (mouseAction.mouseActionType == LEFT_RELEASE)
        {
            handleLeftRelease(mouseAction.xpos, mouseAction.ypos);
        } else {
            assert("unhandled mouse operation");
        }
    }
}

void MouseSystem::LeftPress(double xpos, double ypos, bool shiftPressed, bool altPressed, bool ctrlPressed)
{
    mouseActionQueue.Push(MouseAction(LEFT_PRESS, xpos, ypos));
}

void MouseSystem::LeftRelease(double xpos, double ypos)
{
    mouseActionQueue.Push(MouseAction(LEFT_RELEASE, xpos, ypos));
}

void MouseSystem::Move(double xpos, double ypos)
{
    mouseActionQueue.Push(MouseAction(MOVE, xpos, ypos));
}

void MouseSystem::handleLeftPress(double xpos, double ypos)
{
    SelectCommand selectCommand(componentManager, xpos, ypos, sceneContext.windowWidth, sceneContext.windowHeight, sceneContext.viewMatrix, sceneContext.getPerspectiveProjectionMatrix(), sceneContext.cameraPosition);
    selectCommand.execute();
}

void MouseSystem::handleLeftRelease(double xpos, double ypos)
{
    DeselectCommand deselectCommand(componentManager);
    deselectCommand.execute();
}

void MouseSystem::handleMouseMove(double xpos, double ypos)
{
    MoveCommand moveCommand(componentManager, xpos, ypos, sceneContext.windowWidth, sceneContext.windowHeight, sceneContext.viewMatrix, sceneContext.getPerspectiveProjectionMatrix(), sceneContext.cameraPosition);
    moveCommand.execute();
}
