#include "ComponentManager.h"
#include "SelectedComponent.h" // Component to mark selected entities
#include "TagComponent.h"
#include "Raycaster.h"
#include "EntityUpdatedEvent.h"
#include <mutex>

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
    DeselectCommand(ComponentManager &componentManager) : componentManager(componentManager) {}

    void execute()
    {
        std::unordered_set<Entity> entities;
        {
            std::lock_guard<std::mutex> lock(mutex);
            entities = componentManager.GetEntitiesWithComponent<SelectedComponent>();
        }

        for (auto entity : entities)
        {
            if (componentManager.IsValidEntity(entity))
            {
                if (componentManager.HasComponent<SelectedComponent>(entity))
                    componentManager.RemoveComponent<SelectedComponent>(entity);
                if (componentManager.HasComponent<TagComponent>(entity))
                    componentManager.RemoveComponent<TagComponent>(entity);
            }
        }
    }

private:
    ComponentManager &componentManager;
    std::mutex mutex;
};

class MoveCommand
{
public:
    MoveCommand(ComponentManager &componentManager, double x, double y, float screenWidth, float screenHeight, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition)
        : componentManager(componentManager), x(x), y(y), screenWidth(screenWidth), screenHeight(screenHeight), view(view), projection(projection), cameraPosition(cameraPosition) {}

    void execute()
    {
        Raycaster raycaster;
        glm::vec3 rayDirection = raycaster.screenToWorld(x, y, screenWidth, screenHeight, view, projection);
        glm::vec3 targetPosition = raycaster.getPointOnVirtualPlane(rayDirection, cameraPosition, -5.0f);

        std::unordered_set<Entity> entities;
        {
            std::lock_guard<std::mutex> lock(mutex);
            entities = componentManager.GetEntitiesWithComponent<SelectedComponent>();
        }

        for (auto entity : entities)
        {
            if (componentManager.IsValidEntity(entity) && componentManager.HasComponent<TransformComponent>(entity))
            {
                TransformComponent &transformComponent = componentManager.GetComponent<TransformComponent>(entity);
                transformComponent.position = targetPosition;
                transformComponent.dirty = true;
            }
        }
    }

private:
    ComponentManager &componentManager;
    double x, y;
    float screenWidth, screenHeight;
    glm::mat4 view, projection;
    glm::vec3 cameraPosition;
    std::mutex mutex;
};
