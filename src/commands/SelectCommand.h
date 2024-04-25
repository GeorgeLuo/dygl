#include "ComponentManager.h"
#include "SelectedComponent.h" // Component to mark selected entities
#include "Raycaster.h"

class SelectCommand
{
public:
    // Constructor initializing with the component manager and click coordinates
    SelectCommand(ComponentManager &componentManager, double x, double y, float screenWidth, float screenHeight, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition)
        : componentManager(componentManager), x(x), y(y), screenWidth(screenWidth), screenHeight(screenHeight), view(view), projection(projection), cameraPosition(cameraPosition) {}

    // Method to execute the command
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
    ComponentManager &componentManager; // Reference to component manager
    double x, y;                        // Click coordinates
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
        return selectedEntity; // Placeholder for the actual implementation
    }
};
