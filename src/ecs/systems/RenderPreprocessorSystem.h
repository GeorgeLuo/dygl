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

    void PerformVisibilityCulling();
    void SortEntitiesForEfficientRendering();
    void PrepareLevelOfDetail();
    void SetupShaders();
    void PrepareBatches();
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

    // geometry
    if (!this->componentManager.HasComponent<RenderComponent>(entity))
    {
        unsigned int VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        GLsizei numVertices = 0;
        if (this->componentManager.HasComponent<GeometryComponent>(entity))
        {
            auto &geometry = this->componentManager.GetComponent<GeometryComponent>(entity);
            numVertices = geometry.vertices.size();
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), geometry.vertices.data(), GL_STATIC_DRAW);
        }
        glGenBuffers(1, &VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
        // RenderComponent renderComponent{VAO, VBO};
        // RenderComponent renderComponent{VAO, VBO, numVertices};
        auto renderComponent = RenderComponent(VAO, VBO, numVertices);
        this->componentManager.AddComponent(entity, renderComponent);
    }

    // once per frame environment setup
    auto [lightPos, lightColor] = this->uniformManager.sceneContext.getLightProperties();

    uniformManager.StoreEntityUniforms(entity, "lightPos", std::vector<float>{lightPos[0], lightPos[1], lightPos[2]});
    uniformManager.StoreEntityUniforms(entity, "lightColor", std::vector<float>{lightColor[0], lightColor[1], lightColor[2]});

    // handle entity color
    if (componentManager.HasComponent<ColorComponent>(entity))
    {
        auto &colorComponent = componentManager.GetComponent<ColorComponent>(entity);
        auto color = std::vector<float>{colorComponent.r, colorComponent.g, colorComponent.b, 1.0f};
        uniformManager.StoreEntityUniforms(entity, "ourColor", color);
    }

    glm::mat4 view = this->uniformManager.sceneContext.viewMatrix;
    uniformManager.StoreEntityUniforms(entity, "view", view);

    glm::mat4 projection = this->uniformManager.sceneContext.getPerspectiveProjectionMatrix();
    // if (this->componentManager.HasComponent<TextureComponent>(entity))
    // {
    //     projection = this->uniformManager.sceneContext.getOrthographicProjectionMatrix();
    // }
    uniformManager.StoreEntityUniforms(entity, "projection", projection);

    TransformComponent transform;
    if (componentManager.HasComponent<TransformComponent>(entity))
    {
        transform = componentManager.GetComponent<TransformComponent>(entity);
    }

    glm::mat4 modelMatrix = transform.GetModelMatrix();

    uniformManager.StoreEntityUniforms(entity, "model", modelMatrix);

    if (componentManager.HasComponent<TextureComponent>(entity))
    {
        TextureComponent textureComponent = componentManager.GetComponent<TextureComponent>(entity);
        // Store texture IDs as uniforms; the practical implementation of this will depend on your UniformManager's capabilities
        // For simplicity, assume we store the first texture ID. Adjust based on your needs.
        uniformManager.StoreEntityUniforms(entity, "textureID", textureComponent.textureIDs);
    }
}

void RenderPreprocessorSystem::Update(float deltaTime)
{
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
