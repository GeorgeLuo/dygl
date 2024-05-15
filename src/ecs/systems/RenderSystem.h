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
#include "UniformData.h"
#include "UniformManager.h"
#include "TextureComponent.h"

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
    RenderSystem(EventBus &eventBus, SceneContext &context, UniformManager &uniformManager);
    void Update(float dt, ComponentManager &componentManager);
    void UpdateV2(float dt, ComponentManager &componentManager);
    void UpdateV3(float dt, ComponentManager &componentManager);
    void UpdateV4(float dt, ComponentManager &componentManager);

    void Initialize();
    // void RemoveEntity(Entity entity);

private:
    EventBus &eventBus;
    SceneContext &sceneContext; // Reference to the shared context

    ShaderManager shaderManager;
    UniformManager &uniformManager;

    unsigned int shaderProgram;

    void initializeShaders();

    // per entity helpers
    void setupGeometry(Entity entity, ComponentManager &componentManager);
    void setupShaderWithEntityData(TransformComponent &transform, float angle);
};

RenderSystem::RenderSystem(EventBus &eventBus, SceneContext &context, UniformManager &uniformManager)
    : eventBus(eventBus), sceneContext(context), uniformManager(uniformManager)
{
    this->eventBus.subscribe<EntityCreatedEvent>([this](const EntityCreatedEvent &event)
                                                 { this->AddEntity(event.entity); });

    this->eventBus.subscribe<EntityDestroyedEvent>([this](const EntityDestroyedEvent &event)
                                                   { this->RemoveEntity(event.entity); });
}

void RenderSystem::setupGeometry(Entity entity, ComponentManager &componentManager)
{
    unsigned int VAO, VBO;
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
        // RenderComponent renderComponent{VAO, VBO};
        RenderComponent renderComponent{VAO, VBO, numVertices, 0};
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

void RenderSystem::UpdateV4(float dt, ComponentManager &componentManager)
{
    for (auto entity : this->entities)
    {
        if (!componentManager.HasComponent<ShaderComponent>(entity) ||
            !componentManager.HasComponent<RenderComponent>(entity))
            continue;

        RenderComponent renderComp = componentManager.GetComponent<RenderComponent>(entity);
        ShaderComponent component = componentManager.GetComponent<ShaderComponent>(entity);
        unsigned int program = shaderManager.LoadShaderProgram(
            component.vertexShader,
            component.fragmentShader);
        CheckGLError();

        shaderManager.UseShader(program);

        glBindVertexArray(renderComp.VAO);

        // Bind texture if available
        if (componentManager.HasComponent<TextureComponent>(entity))
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            TextureComponent &textureComp = componentManager.GetComponent<TextureComponent>(entity);
            // glBindTexture(GL_TEXTURE_2D, textureComp.textureID);

            // Bind the texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureComp.textureIDs[0]);
            glUniform1i(glGetUniformLocation(program, "textTexture"), 0); // Texture unit 0

            // Set the text color, example: white
            glUniform4f(glGetUniformLocation(program, "textColor"), 1.0f, 1.0f, 1.0f, 1.0f);
        }

        // Set uniforms
        UniformData uniforms = uniformManager.GetUniforms(entity);
        // Uniform handling remains unchanged
        for (auto &[key, val] : uniforms.floatVecUniforms)
        {
            if (val.size() == 3)
            {
                shaderManager.SetUniform3f(program, key, val[0], val[1], val[2]);
            }
            else if (val.size() == 4)
            {
                shaderManager.SetUniform4fv(program, key, glm::vec4(val[0], val[1], val[2], val[3]));
            }
        }

        for (auto &[key, val] : uniforms.mat4Uniforms)
        {
            shaderManager.SetUniformMatrix4fv(program, key, glm::value_ptr(val));
        }

        GLsizei numVertices = componentManager.HasComponent<GeometryComponent>(entity)
                                  ? componentManager.GetComponent<GeometryComponent>(entity).vertices.size()
                                  : 0;

        glDrawArrays(GL_TRIANGLES, 0, renderComp.vertexCount);
        CheckGLError();
    }
}

void RenderSystem::UpdateV3(float dt, ComponentManager &componentManager)
{
    for (auto entity : this->entities)
    {
        // get the shader from ShaderComponent and use shader

        if (!componentManager.HasComponent<ShaderComponent>(entity))
            continue;

        ShaderComponent component = componentManager.GetComponent<ShaderComponent>(entity);
        unsigned int program = shaderManager.LoadShaderProgram(
            component.vertexShader,
            component.fragmentShader);
        CheckGLError();

        shaderManager.UseShader(program);

        // get the uniforms from UniformComponent and set uniforms to shader

        UniformData uniforms = uniformManager.GetUniforms(entity);

        for (auto &[key, val] : uniforms.floatVecUniforms)
        {
            if (val.size() == 3)
            {
                shaderManager.SetUniform3f(program, key, val[0], val[1], val[2]);
            }
            else if (val.size() == 4)
            {
                shaderManager.SetUniform4fv(program, key, glm::vec4(val[0], val[1], val[2], val[3]));
            }
        }

        for (auto &[key, val] : uniforms.mat4Uniforms)
        {
            shaderManager.SetUniformMatrix4fv(program, key, glm::value_ptr(val));
        }

        // bind VAO from RenderComponent

        auto &renderComponent = componentManager.GetComponent<RenderComponent>(entity);
        glBindVertexArray(renderComponent.VAO);

        GLsizei numVertices = 0;
        if (componentManager.HasComponent<GeometryComponent>(entity))
        {
            auto &geometry = componentManager.GetComponent<GeometryComponent>(entity);
            numVertices = geometry.vertices.size();
        }
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
        CheckGLError();
    }
}

void RenderSystem::Update(float dt, ComponentManager &componentManager)
{
    // glEnable(GL_DEPTH_TEST);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

        auto [lightPos, lightColor] = sceneContext.getLightProperties();

        int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

        int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

        // Pass view and projection matrices to the shader
        glm::mat4 view = sceneContext.viewMatrix;
        glm::mat4 projection = sceneContext.getPerspectiveProjectionMatrix();

        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        TransformComponent transform;
        if (componentManager.HasComponent<TransformComponent>(entity))
        {
            transform = componentManager.GetComponent<TransformComponent>(entity);
        }

        float currentTime = glfwGetTime();
        float angle = currentTime;

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