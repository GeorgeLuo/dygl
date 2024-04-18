#pragma once
#include "ComponentManager.h"
#include "ConcurrentQueue.h"
#include "EntityManager.h"
#include "RenderSystem.h"
#include <glad.h> // Include GLAD before GLFW.
#include <glfw3.h>
#include <iostream>
#include <tuple>
#include "ColorComponent.h"
#include "TagComponent.h"
#include "QueueCollection.h"
#include "IdComponent.h"
#include "ChangeColorCommand.h"
#include "ThreeDComponent.h"

class OpenGLApp
{
public:
    EntityManager entityManager;
    ComponentManager componentManager;
    RenderSystem renderSystem;
    QueueCollection &queueCollection;
    std::unordered_map<int, Entity> idToEntityMap;

    OpenGLApp(QueueCollection &queueCollectionRef)
        : queueCollection(queueCollectionRef) {}

    static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    Entity findEntityById(int id)
    {
        auto it = idToEntityMap.find(id);

        if (it != idToEntityMap.end())
        {
            return it->second;
        }
        else
        {
            return INVALID_ENTITY;
        }
    }

    void createEntity(const EntityCreationMessage &message)
    {
        Entity newEntity = entityManager.CreateEntity();

        // Add IdComponent with external ID
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
        renderSystem.AddEntity(newEntity);
        // Add other components as necessary, for example, a RenderComponent might be initialized here
        // componentManager.AddComponent(newEntity, RenderComponent(...));

        // If you need to track entity types or other properties, you can modify or add other components as needed

        idToEntityMap[message.id] = newEntity;
    }

    void deleteEntity(int id)
    {
        auto it = idToEntityMap.find(id);
        if (it != idToEntityMap.end())
        {
            Entity entity = it->second;

            // Inform RenderSystem about the entity being removed
            renderSystem.RemoveEntity(entity); // Assuming RenderSystem has a method RemoveEntity

            // Remove the associated components, for example:
            componentManager.RemoveComponent<RenderComponent>(entity);
            // Repeat the above line for other component types the entity may have

            // Now, properly destroy the entity and remove from the map
            entityManager.DestroyEntity(entity);
            idToEntityMap.erase(it);
        }
    }

    void Initialize()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For macOS
#endif
        window = glfwCreateWindow(800, 600, "Render System Usage", NULL, NULL);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        if (window == NULL)
        {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            exit(-1);
        }
        glfwMakeContextCurrent(window);

        // GLAD: load all OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            exit(-1);
        }
        renderSystem.Initialize();
    }

    void Run()
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        // Rendering loop
        while (!glfwWindowShouldClose(window))
        {
            std::vector<EntityCreationMessage> creationMessages;
            while (queueCollection.entityCreationQueue.TryPop(creationMessages))
            {
                for (const auto &message : creationMessages)
                {
                    createEntity(message);
                }
            }

            std::vector<EntityDeletionMessage> deletionMessages;
            while (queueCollection.entityDeletionQueue.TryPop(deletionMessages))
            {
                for (const auto &message : deletionMessages)
                {
                    deleteEntity(message.id);
                }
            }

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

            glClear(GL_COLOR_BUFFER_BIT);

            // Update the RenderSystem - this will handle all rendering for entities it knows about
            renderSystem.Update(0.016f, componentManager); // For this example, assuming a fixed timestep
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        // Cleanup GLFW
        glfwDestroyWindow(window);
        glfwTerminate();
    }

private:
    GLFWwindow *window;
};