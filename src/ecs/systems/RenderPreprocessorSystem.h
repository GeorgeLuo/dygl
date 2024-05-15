#pragma once

#include "System.h"
#include <vector>
#include "ComponentManager.h"
#include "SceneMetaChangeEvent.h"
#include "EntityUpdatedEvent.h"

class RenderPreprocessorSystem : public System
{
public:
    RenderPreprocessorSystem(ComponentManager &componentManager);
    RenderPreprocessorSystem(EventBus &eventBus, ComponentManager &componentManager, UniformManager &uniformManager);

    // virtual ~RenderPreprocessorSystem() override;
    void AddEntity(Entity entity);
    void UpdateEntity(Entity entity);
    void Update(float deltaTime) override;

    void Initialize();
    // void Update(float dt);
    void PrepareForRendering();

private:
    UniformManager &uniformManager;
    EventBus &eventBus;
    ComponentManager &componentManager;
};

RenderPreprocessorSystem::RenderPreprocessorSystem(EventBus &eventBus, ComponentManager &componentManager, UniformManager &uniformManager)
    : eventBus(eventBus), componentManager(componentManager), uniformManager(uniformManager)
{
    this->eventBus.subscribe<EntityCreatedEvent>([this](const EntityCreatedEvent &event)
                                                 { this->AddEntity(event.entity); });

    this->eventBus.subscribe<EntityUpdatedEvent>([this](const EntityUpdatedEvent &event)
                                                 { this->UpdateEntity(event.entity); });

    this->eventBus.subscribe<EntityDestroyedEvent>([this](const EntityDestroyedEvent &event)
                                                   { this->RemoveEntity(event.entity); });

    this->eventBus.subscribe<SceneMetaChangeEvent>([this](const SceneMetaChangeEvent &event)
                                                   { this->RemoveEntity(event.entity); });
}

void RenderPreprocessorSystem::AddEntity(Entity entity)
{
    System::AddEntity(entity);

    if (!this->componentManager.HasComponent<RenderComponent>(entity))
    {
        unsigned int VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        GLsizei numVertices = 0;
        GLsizeiptr bufferSize = 0;

        if (this->componentManager.HasComponent<GeometryComponent>(entity))
        {
            auto &geometry = this->componentManager.GetComponent<GeometryComponent>(entity);
            numVertices = geometry.vertices.size();
            bufferSize = numVertices * sizeof(Vertex);
            glBufferData(GL_ARRAY_BUFFER, bufferSize, geometry.vertices.data(), GL_STATIC_DRAW);
        }

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, x)); // Position
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, u)); // Texture coordinates
        glEnableVertexAttribArray(1);

        auto renderComponent = RenderComponent(VAO, VBO, numVertices, bufferSize);
        this->componentManager.AddComponent(entity, renderComponent);
    }

    auto [lightPos, lightColor] = this->uniformManager.sceneContext.getLightProperties();
    uniformManager.StoreEntityUniforms(entity, "lightPos", std::vector<float>{lightPos[0], lightPos[1], lightPos[2]});
    uniformManager.StoreEntityUniforms(entity, "lightColor", std::vector<float>{lightColor[0], lightColor[1], lightColor[2]});

    if (componentManager.HasComponent<ColorComponent>(entity))
    {
        auto &colorComponent = componentManager.GetComponent<ColorComponent>(entity);
        auto color = std::vector<float>{colorComponent.r, colorComponent.g, colorComponent.b, 1.0f};
        uniformManager.StoreEntityUniforms(entity, "ourColor", color);
    }

    glm::mat4 view = this->uniformManager.sceneContext.viewMatrix;
    uniformManager.StoreEntityUniforms(entity, "view", view);

    glm::mat4 projection = this->uniformManager.sceneContext.getPerspectiveProjectionMatrix();
    uniformManager.StoreEntityUniforms(entity, "projection", projection);

    if (componentManager.HasComponent<TransformComponent>(entity))
    {
        TransformComponent &transform = componentManager.GetComponent<TransformComponent>(entity);
        glm::mat4 modelMatrix = transform.GetModelMatrix();
        uniformManager.StoreEntityUniforms(entity, "model", modelMatrix);
    }

    if (componentManager.HasComponent<TextureComponent>(entity))
    {
        TextureComponent textureComponent = componentManager.GetComponent<TextureComponent>(entity);
        uniformManager.StoreEntityUniforms(entity, "textureID", textureComponent.textureIDs);
    }
}

void RenderPreprocessorSystem::Update(float deltaTime)
{
    // Iterate through entities and update OpenGL buffers if necessary
    for (auto entity : this->entities)
    {
        if (componentManager.HasComponent<GeometryComponent>(entity) && componentManager.HasComponent<RenderComponent>(entity))
        {
            auto &geometry = componentManager.GetComponent<GeometryComponent>(entity);
            auto &render = componentManager.GetComponent<RenderComponent>(entity);

            if (geometry.dirty)
            {
                glBindVertexArray(render.VAO);
                glBindBuffer(GL_ARRAY_BUFFER, render.VBO);

                if (geometry.vertices.size() * sizeof(Vertex) > render.bufferSize)
                {
                    glBufferData(GL_ARRAY_BUFFER, geometry.vertices.size() * sizeof(Vertex), geometry.vertices.data(), GL_STATIC_DRAW);
                    render.bufferSize = geometry.vertices.size() * sizeof(Vertex);
                }
                else
                {
                    glBufferSubData(GL_ARRAY_BUFFER, 0, geometry.vertices.size() * sizeof(Vertex), geometry.vertices.data());
                }

                render.vertexCount = geometry.vertices.size();
                geometry.dirty = false;

                glBindVertexArray(0);
            }
        }
    }
}

void RenderPreprocessorSystem::UpdateEntity(Entity entity)
{
    TransformComponent transform;
    if (componentManager.HasComponent<TransformComponent>(entity))
    {
        transform = componentManager.GetComponent<TransformComponent>(entity);
    }

    glm::mat4 modelMatrix = transform.GetModelMatrix();
    uniformManager.StoreEntityUniforms(entity, "model", modelMatrix);
}
