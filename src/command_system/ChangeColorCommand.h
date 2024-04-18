#pragma once
#include "ICommand.h"
#include "ComponentManager.h" // Assuming this manages components.
#include "ColorComponent.h"

class ChangeColorCommand : public ICommand
{
private:
    ComponentManager &componentManager; // Reference to ComponentManager
    Entity entity;                      // Entity to change color of
    float r, g, b;                      // New color values

public:
    ChangeColorCommand(ComponentManager &componentMgr, Entity entity, float r, float g, float b)
        : componentManager(componentMgr), entity(entity), r(r), g(g), b(b) {}

    void execute() override
    {
        if (componentManager.HasComponent<ColorComponent>(entity))
        {
            auto &colorComponent = componentManager.GetComponent<ColorComponent>(entity);
            // Set the target color to the new color values
            colorComponent.SetTargetColor(r, g, b);
        }
        else
        {
            // If there's no ColorComponent, add it with the target color
            componentManager.AddComponent(entity, ColorComponent(r, g, b));
        }
    }
};