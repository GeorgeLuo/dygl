#include "ColorComponent.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "GeometryComponent.h"
#include "System.h"
#include "TransformComponent.h"
#include <glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include "ShaderManager.h"
#include "RenderComponent.h"
#include "ShaderComponent.h"
#include <glfw3.h>
#include "SceneContext.h"

void CheckGLError()
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error: " << err << std::endl;
        switch (err)
        {
        case GL_INVALID_ENUM:
            std::cerr << "GL_INVALID_ENUM" << std::endl;
            break;
        case GL_INVALID_VALUE:
            std::cerr << "GL_INVALID_VALUE" << std::endl;
            break;
        case GL_INVALID_OPERATION:
            std::cerr << "GL_INVALID_OPERATION" << std::endl;
            break;
        case GL_OUT_OF_MEMORY:
            std::cerr << "GL_OUT_OF_MEMORY" << std::endl;
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
            break;
        default:
            std::cerr << "Unknown error" << std::endl;
        }
    }
}

class RenderSystem : public System
{
public:
    RenderSystem(EventBus &eventBus, SceneContext &context);
    void Update(float dt, ComponentManager &componentManager);
    void Initialize();
    void RemoveEntity(Entity entity);

private:
    EventBus &eventBus;
    SceneContext &sceneContext; // Reference to the shared context

    ShaderManager shaderManager;

    unsigned int shaderProgram;

    void initializeShaders();

    // per entity helpers
    void setupGeometry(Entity entity, ComponentManager &componentManager);
    void setupShaderWithEntityData(TransformComponent &transform, float angle);
};

RenderSystem::RenderSystem(EventBus &eventBus, SceneContext &context)
    : eventBus(eventBus), sceneContext(context)
{
    this->eventBus.subscribe<EntityCreatedEvent>([this](const EntityCreatedEvent &event)
                                                 { this->AddEntity(event.entity); });

    this->eventBus.subscribe<EntityDestroyedEvent>([this](const EntityDestroyedEvent &event)
                                                   { this->RemoveEntity(event.entity); });
}

void RenderSystem::setupGeometry(Entity entity, ComponentManager &componentManager)
{
    if (!componentManager.HasComponent<RenderComponent>(entity))
    {
        unsigned int VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);              // Generate VBO before using it
        glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind the buffer

        GLsizei numVertices = 0;
        if (componentManager.HasComponent<GeometryComponent>(entity))
        {
            auto &geometry = componentManager.GetComponent<GeometryComponent>(entity);
            numVertices = geometry.vertices.size();
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), geometry.vertices.data(), GL_STATIC_DRAW);
        }
        glGenBuffers(1, &VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0); // Unbind VAO to prevent further modifications.
        // Store the VAO and VBO in RenderComponent
        RenderComponent renderComponent{VAO, VBO};
        componentManager.AddComponent(entity, renderComponent);
        // Keep track that we've added the component to avoid duplication in future updates
    }
}

void RenderSystem::initializeShaders()
{
    CheckGLError();
}

void RenderSystem::Initialize()
{
    initializeShaders();
    CheckGLError();

    glBindVertexArray(0); // Unbind the VAO to prevent unintended modifications.
}

void RenderSystem::Update(float dt, ComponentManager &componentManager)
{
    auto [lightPos, lightColor] = sceneContext.getLightProperties();

    float currentTime = glfwGetTime();
    float angle = currentTime;

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = sceneContext.viewMatrix;
    glm::mat4 projection = sceneContext.projectionMatrix;

    for (auto entity : this->entities)
    {
        if (!componentManager.HasComponent<GeometryComponent>(entity))
            continue; // Skip entities without relevant components

        if (componentManager.HasComponent<ShaderComponent>(entity))
        {
            ShaderComponent component = componentManager.GetComponent<ShaderComponent>(entity);
            shaderProgram = shaderManager.LoadShaderProgram(
                component.vertexShader,
                component.fragmentShader);
            CheckGLError();
        }

        shaderManager.UseShader(shaderProgram);

        int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

        int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

        // Pass view and projection matrices to the shader
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        if (!(componentManager.HasComponent<GeometryComponent>(entity)))
            continue; // Skip entities without geometry or 3D components

        TransformComponent transform;
        if (componentManager.HasComponent<TransformComponent>(entity))
        {
            transform = componentManager.GetComponent<TransformComponent>(entity);
        }
        setupShaderWithEntityData(transform, angle);
        setupGeometry(entity, componentManager);

        glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f}; // Default color: white
        if (componentManager.HasComponent<ColorComponent>(entity))
        {
            auto &colorComponent = componentManager.GetComponent<ColorComponent>(entity);
            color = glm::vec4(colorComponent.r, colorComponent.g, colorComponent.b, 1.0f);
        }

        // Pass color to the shader
        int colorLoc = glGetUniformLocation(shaderProgram, "ourColor");
        glUniform4fv(colorLoc, 1, glm::value_ptr(color));

        auto &renderComponent = componentManager.GetComponent<RenderComponent>(entity);
        glBindVertexArray(renderComponent.VAO);
        GLsizei numVertices = 0;
        if (componentManager.HasComponent<GeometryComponent>(entity))
        {
            auto &geometry = componentManager.GetComponent<GeometryComponent>(entity);
            numVertices = geometry.vertices.size();
        }
        glDrawArrays(GL_TRIANGLES, 0, numVertices); // Use the appropriate number of vertices here
        CheckGLError();
    }
}

void RenderSystem::RemoveEntity(Entity entity)
{
    entities.erase(entity);
}

void RenderSystem::setupShaderWithEntityData(TransformComponent &transform, float angle)
{
    glm::mat4 modelMatrix = transform.GetModelMatrix(); // Start with the original transform

    // if (shaderProgram == threeDShaderProgram)
    {
        // New diagonal axis for rotation (e.g., (1, 1, 1))
        glm::vec3 diagonalAxis(1.0f, 1.0f, 1.0f); // Diagonal axis combining X, Y, and Z

        // Normalize the axis to ensure uniform rotation
        diagonalAxis = glm::normalize(diagonalAxis);

        // Apply the rotation
        modelMatrix = glm::rotate(
            modelMatrix,                 // Start with the current model matrix
            glm::radians(angle * 50.0f), // Angle of rotation (radians per second)
            diagonalAxis                 // Diagonal axis for rotation
        );
    }

    // Update the shader uniform for the model matrix
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
}
