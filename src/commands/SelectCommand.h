#include "ComponentManager.h"
#include "SelectedComponent.h" // Component to mark selected entities
#include "TagComponent.h"
#include "Raycaster.h"

class SelectCommand
{
public:
    SelectCommand(ComponentManager &componentManager, double x, double y, float screenWidth, float screenHeight, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition)
        : componentManager(componentManager), x(x), y(y), screenWidth(screenWidth), screenHeight(screenHeight), view(view), projection(projection), cameraPosition(cameraPosition) {}

    void execute()
    {
        // Find the entity that corresponds to the click coordinates
        Entity selectedEntity = findEntityAtCoordinates();
        if (selectedEntity != INVALID_ENTITY)
        {
            componentManager.AddComponent(selectedEntity, SelectedComponent());
            TagComponent tagComponent;
            tagComponent.AddTag("shape");
            componentManager.AddComponent(selectedEntity, tagComponent);
        }
    }

private:
    ComponentManager &componentManager;
    double x, y;
    float screenWidth, screenHeight;
    glm::mat4 view, projection;
    glm::vec3 cameraPosition;

    // Function to find an entity at the specified coordinates
    Entity findEntityAtCoordinates()
    {
        Raycaster raycaster;
        glm::vec3 rayDirection = raycaster.screenToWorld(x, y, screenWidth, screenHeight, view, projection);
        glm::vec3 rayOrigin = cameraPosition;
        Entity selectedEntity = raycaster.raycast(rayOrigin, rayDirection, componentManager);
        return selectedEntity;
    }
};

class DeselectCommand
{
public:
    DeselectCommand(ComponentManager &componentManager) : componentManager(componentManager){};
    void execute()
    {
        for (auto entity : componentManager.GetEntitiesWithComponent<SelectedComponent>())
        {
            if (componentManager.HasComponent<SelectedComponent>(entity))
                componentManager.RemoveComponent<SelectedComponent>(entity);
            if (componentManager.HasComponent<TagComponent>(entity))
                componentManager.RemoveComponent<TagComponent>(entity);
        }
    }

private:
    ComponentManager &componentManager;
};

class MoveCommand
{
public:
    MoveCommand(ComponentManager &componentManager, double x, double y, float screenWidth, float screenHeight, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition)
        : componentManager(componentManager), x(x), y(y), screenWidth(screenWidth), screenHeight(screenHeight), view(view), projection(projection), cameraPosition(cameraPosition) {}
    void execute()
    {
        Raycaster raycaster;
        glm::vec3 rayDirection = raycaster.screenToWorld(
            x, y, screenWidth, screenHeight, view, projection);
        glm::vec3 targetPosition = raycaster.getPointOnVirtualPlane(rayDirection, cameraPosition, -5.0f);

        for (auto entity : componentManager.GetEntitiesWithComponent<SelectedComponent>())
        {
            if (componentManager.HasComponent<TransformComponent>(entity))
            {
                TransformComponent &transformComponent = componentManager.GetComponent<TransformComponent>(entity); // Note the & to get a reference
                transformComponent.position = targetPosition;
            }
        }
    }

private:
    ComponentManager &componentManager;
    double x, y;
    float screenWidth, screenHeight;
    glm::mat4 view, projection;
    glm::vec3 cameraPosition;
};