#pragma once

#include "System.h"
#include <vector>
#include "ComponentManager.h"
#include "SceneMetaChangeEvent.h"
#include "EntityUpdatedEvent.h"
#include "TextBlockComponent.h"

class RenderPreprocessorSystem : public System
{
public:
    // RenderPreprocessorSystem(EventBus &eventBus, ComponentManager &componentManager, UniformManager &uniformManager);
    RenderPreprocessorSystem(ComponentManager &componentManager, UniformManager &uniformManager);

    void setupVisibility(Entity entity);
    void Update(float deltaTime) override;

private:
    UniformManager &uniformManager;
    // EventBus &eventBus;
    ComponentManager &componentManager;

    void updateEntity(Entity entity);
    void updateEntityColor(Entity entity);
};

// RenderPreprocessorSystem::RenderPreprocessorSystem(EventBus &eventBus, ComponentManager &componentManager, UniformManager &uniformManager)
//     : eventBus(eventBus), componentManager(componentManager), uniformManager(uniformManager)
RenderPreprocessorSystem::RenderPreprocessorSystem(ComponentManager &componentManager, UniformManager &uniformManager)
    : componentManager(componentManager), uniformManager(uniformManager)
{
    // this->eventBus.subscribe<EntityCreatedEvent>([this](const EntityCreatedEvent &event)
    //                                              { this->AddEntity(event.entity); });

    // this->eventBus.subscribe<EntityUpdatedEvent>([this](const EntityUpdatedEvent &event)
    //                                              { this->updateEntity(event.entity); });

    // this->eventBus.subscribe<EntityDestroyedEvent>([this](const EntityDestroyedEvent &event)
    //                                                { this->RemoveEntity(event.entity); });

    // this->eventBus.subscribe<SceneMetaChangeEvent>([this](const SceneMetaChangeEvent &event)
    //                                                { this->RemoveEntity(event.entity); });
}

void RenderPreprocessorSystem::setupVisibility(Entity entity)
{
    // System::AddEntity(entity);

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

    auto [lightPos, lightColor] = this->uniformManager.GetSceneContext().getLightProperties();
    uniformManager.StoreEntityUniforms(entity, "lightPos", std::vector<float>{lightPos[0], lightPos[1], lightPos[2]});
    uniformManager.StoreEntityUniforms(entity, "lightColor", std::vector<float>{lightColor[0], lightColor[1], lightColor[2]});

    if (componentManager.HasComponent<ColorComponent>(entity))
    {
        auto &colorComponent = componentManager.GetComponent<ColorComponent>(entity);
        auto color = std::vector<float>{colorComponent.r, colorComponent.g, colorComponent.b, 1.0f};
        uniformManager.StoreEntityUniforms(entity, "ourColor", color);
    }

    glm::mat4 view = this->uniformManager.GetSceneContext().viewMatrix;
    uniformManager.StoreEntityUniforms(entity, "view", view);

    glm::mat4 projection = this->uniformManager.GetSceneContext().getPerspectiveProjectionMatrix();
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

    // TODO: getting too detailed here. Maybe change to a plugin system
    if (componentManager.HasComponent<TextBlockComponent>(entity))
    {
        auto text = componentManager.GetComponent<TextBlockComponent>(entity);
        uniformManager.StoreEntityUniforms(entity, "textTexture", text.texture);
        uniformManager.StoreEntityUniforms(entity, "ourColor", text.color);
    }
}

void RenderPreprocessorSystem::Update(float deltaTime)
{
    // for (auto entity : this->entities)
    for (auto entity : componentManager.GetEntitiesWithComponent<GeometryComponent>())
    {
        // if (componentManager.HasComponent<EntityUpdatedComponent>(entity))
        // {
        //     updateEntity(entity);
        // }

        // the entity is meant to be rendered by the geometry component but does not
        // have the renderable component set up
        // TODO: add a hide component for performance
        if (!componentManager.HasComponent<RenderComponent>(entity))
        {
            setupVisibility(entity);
        }

        if (componentManager.HasComponent<ColorComponent>(entity) && componentManager.GetComponent<ColorComponent>(entity).dirty)
        {
            updateEntityColor(entity);
        }

        if (componentManager.GetComponent<TransformComponent>(entity).dirty)
        {
            updateEntity(entity);
        }

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

void RenderPreprocessorSystem::updateEntity(Entity entity)
{
    // TransformComponent transform;
    // if (componentManager.HasComponent<TransformComponent>(entity))
    // {
    //     transform = componentManager.GetComponent<TransformComponent>(entity);
    // }

    TransformComponent &transform = componentManager.GetComponent<TransformComponent>(entity);

    glm::mat4 modelMatrix = transform.GetModelMatrix();
    uniformManager.StoreEntityUniforms(entity, "model", modelMatrix);

    transform.dirty = false;
}

void RenderPreprocessorSystem::updateEntityColor(Entity entity)
{
    if (componentManager.HasComponent<ColorComponent>(entity))
    {
        auto &colorComponent = componentManager.GetComponent<ColorComponent>(entity);
        auto color = std::vector<float>{colorComponent.r, colorComponent.g, colorComponent.b, 1.0f};
        uniformManager.StoreEntityUniforms(entity, "ourColor", color);
        colorComponent.dirty = false;
    }
}