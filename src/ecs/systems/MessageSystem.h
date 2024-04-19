#include "QueueCollection.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "ColorComponent.h"
#include "ChangeColorCommand.h"
#include "TagComponent.h"
#include "System.h"
#include <tuple>
#include "EventBus.h"

class MessageSystem : public System
{
public:
    EntityManager &entityManager;
    ComponentManager &componentManager;
    QueueCollection &queueCollection;
    EventBus &eventBus;

    std::unordered_map<int, Entity> idToEntityMap;

    MessageSystem(EntityManager &entityManager, ComponentManager &componentManager, QueueCollection &queueCollection, EventBus &eventBus)
        : entityManager(entityManager), componentManager(componentManager), queueCollection(queueCollection), eventBus(eventBus) {}

    void Update(float deltaTime) override
    {
        ProcessCreationMessages();
        ProcessDeletionMessages();
        ProcessColorChangeMessages();
    }

private:
    void ProcessCreationMessages()
    {
        std::vector<EntityCreationMessage> creationMessages;
        while (queueCollection.entityCreationQueue.TryPop(creationMessages))
        {
            for (const auto &message : creationMessages)
            {
                // You can implement the entity creation logic here
                // For example, call an existing function: createEntity(message);
                Entity newEntity = entityManager.CreateEntity();

                componentManager.AddComponent(newEntity, IdComponent(message.id));

                // Initialize the TransformComponent based on message position
                componentManager.AddComponent(newEntity, TransformComponent(message.x, message.y, message.z));

                TagComponent tagComponent;
                tagComponent.AddTag("shape");

                if (message.shape == "square")
                {
                    std::vector<Vertex> squareVertices = {
                        Vertex(-0.5f, -0.5f, 0.0f), // Bottom left
                        Vertex(0.5f, -0.5f, 0.0f),  // Bottom right
                        Vertex(0.5f, 0.5f, 0.0f),   // Top right

                        Vertex(-0.5f, -0.5f, 0.0f), // Bottom left
                        Vertex(0.5f, 0.5f, 0.0f),   // Top right
                        Vertex(-0.5f, 0.5f, 0.0f)   // Top left
                    };
                    GeometryComponent geometrySquare(squareVertices);
                    componentManager.AddComponent(newEntity, geometrySquare);
                }
                else if (message.shape == "triangle")
                {
                    std::vector<Vertex> triangleVertices = {
                        Vertex(-0.5f, -0.5f, 0.0f),
                        Vertex(0.5f, -0.5f, 0.0f),
                        Vertex(0.0f, 0.5f, 0.0f),
                    };
                    GeometryComponent triangleGeometry(triangleVertices);
                    componentManager.AddComponent(newEntity, triangleGeometry);
                }
                else if (message.shape == "pyramid")
                {
                    // Pyramid vertices
                    std::vector<Vertex> pyramidVertices = {
                        // Base square face (assuming it lies on the XY plane at Z = -0.5)
                        Vertex(-0.5f, -0.5f, -0.5f), Vertex(0.5f, -0.5f, -0.5f), Vertex(0.5f, 0.5f, -0.5f),
                        Vertex(-0.5f, 0.5f, -0.5f), Vertex(-0.5f, -0.5f, -0.5f), Vertex(0.5f, 0.5f, -0.5f),
                        // Triangle face 1 (Front)
                        Vertex(-0.5f, -0.5f, -0.5f), Vertex(0.5f, -0.5f, -0.5f), Vertex(0.0f, 0.0f, 0.5f),
                        // Triangle face 2 (Right)
                        Vertex(0.5f, -0.5f, -0.5f), Vertex(0.5f, 0.5f, -0.5f), Vertex(0.0f, 0.0f, 0.5f),
                        // Triangle face 3 (Back)
                        Vertex(0.5f, 0.5f, -0.5f), Vertex(-0.5f, 0.5f, -0.5f), Vertex(0.0f, 0.0f, 0.5f),
                        // Triangle face 4 (Left)
                        Vertex(-0.5f, 0.5f, -0.5f), Vertex(-0.5f, -0.5f, -0.5f), Vertex(0.0f, 0.0f, 0.5f)};
                    ThreeDComponent pyramidComponent(pyramidVertices);
                    componentManager.AddComponent(newEntity, pyramidComponent);
                    // Add any other relevant components such as transform, rendering, etc.
                }
                else if (message.shape == "cube")
                {
                    // Cube vertices
                    std::vector<Vertex> cubeVertices = {
                        // Front face
                        Vertex(-0.5f, -0.5f, 0.5f), Vertex(0.5f, -0.5f, 0.5f), Vertex(0.5f, 0.5f, 0.5f),
                        Vertex(-0.5f, 0.5f, 0.5f), Vertex(-0.5f, -0.5f, 0.5f), Vertex(0.5f, 0.5f, 0.5f),
                        // Right face
                        Vertex(0.5f, -0.5f, 0.5f), Vertex(0.5f, -0.5f, -0.5f), Vertex(0.5f, 0.5f, -0.5f),
                        Vertex(0.5f, 0.5f, -0.5f), Vertex(0.5f, 0.5f, 0.5f), Vertex(0.5f, -0.5f, 0.5f),
                        // Back face
                        Vertex(-0.5f, -0.5f, -0.5f), Vertex(0.5f, -0.5f, -0.5f), Vertex(0.5f, 0.5f, -0.5f),
                        Vertex(-0.5f, 0.5f, -0.5f), Vertex(-0.5f, -0.5f, -0.5f), Vertex(0.5f, 0.5f, -0.5f),
                        // Left face
                        Vertex(-0.5f, -0.5f, -0.5f), Vertex(-0.5f, -0.5f, 0.5f), Vertex(-0.5f, 0.5f, 0.5f),
                        Vertex(-0.5f, 0.5f, -0.5f), Vertex(-0.5f, -0.5f, -0.5f), Vertex(-0.5f, 0.5f, 0.5f),
                        // Top face
                        Vertex(-0.5f, 0.5f, 0.5f), Vertex(0.5f, 0.5f, 0.5f), Vertex(0.5f, 0.5f, -0.5f),
                        Vertex(-0.5f, 0.5f, -0.5f), Vertex(-0.5f, 0.5f, 0.5f), Vertex(0.5f, 0.5f, -0.5f),
                        // Bottom face
                        Vertex(-0.5f, -0.5f, -0.5f), Vertex(0.5f, -0.5f, -0.5f), Vertex(0.5f, -0.5f, 0.5f),
                        Vertex(-0.5f, -0.5f, 0.5f), Vertex(-0.5f, -0.5f, -0.5f), Vertex(0.5f, -0.5f, 0.5f)};
                    ThreeDComponent cubeComponent(cubeVertices);
                    componentManager.AddComponent(newEntity, cubeComponent);
                }
                componentManager.AddComponent(newEntity, tagComponent);

                // renderSystem.AddEntity(newEntity);

                idToEntityMap[message.id] = newEntity;
            }
        }
    }

    void ProcessDeletionMessages()
    {
        std::vector<EntityDeletionMessage> deletionMessages;
        while (queueCollection.entityDeletionQueue.TryPop(deletionMessages))
        {
            for (const auto &message : deletionMessages)
            {
                auto it = idToEntityMap.find(message.id);
                if (it != idToEntityMap.end())
                {
                    Entity entity = it->second;

                    // Inform RenderSystem about the entity being removed
                    // renderSystem.RemoveEntity(entity); // Assuming RenderSystem has a method RemoveEntity

                    // Remove the associated components, for example:
                    // componentManager.RemoveComponent<RenderComponent>(entity);
                    componentManager.RemoveAllComponents(entity);
                    // Repeat the above line for other component types the entity may have

                    // Now, properly destroy the entity and remove from the map
                    entityManager.DestroyEntity(entity);
                    idToEntityMap.erase(it);
                }
            }
        }
    }

    void ProcessColorChangeMessages()
    {
        std::tuple<float, float, float> color;
        while (queueCollection.colorQueue.TryPop(color))
        {
            for (auto entity : componentManager.GetEntitiesWithComponent<TagComponent>())
            {
                if (componentManager.GetComponent<TagComponent>(entity).HasTag("shape"))
                {
                    {
                        // Create and execute a ChangeColorCommand for each entity
                        ChangeColorCommand changeColorCmd(
                            componentManager,
                            entity,
                            std::get<0>(color), std::get<1>(color), std::get<2>(color));
                        changeColorCmd.execute();
                    }
                }
            }
        }

        // Update the color of each entity towards its target color
        auto colorEntities = componentManager.GetEntitiesWithComponent<ColorComponent>();
        for (auto entity : colorEntities)
        {
            auto &colorComponent = componentManager.GetComponent<ColorComponent>(entity);
            colorComponent.UpdateColor(); // Gradually update color towards target
        }
    }
};
