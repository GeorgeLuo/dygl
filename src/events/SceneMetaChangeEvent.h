#pragma once

struct SceneMetaChangeEvent : Event
{
    Entity entity;

    SceneMetaChangeEvent(Entity entity) : entity(entity) {}
};

struct GlobalEntityTransformEvent : Event
{
    Entity entity;
    glm::mat4 transform;

    GlobalEntityTransformEvent(Entity entity, glm::mat4 transform) : entity(entity), transform(transform) {}
};

struct LightingEvent : Event
{
    Entity lightEntity;
    glm::vec3 lightColor;
    float lightIntensity;

    LightingEvent(Entity lightEntity, glm::vec3 lightColor, float lightIntensity) 
        : lightEntity(lightEntity), lightColor(lightColor), lightIntensity(lightIntensity) {}
};

struct CameraEvent : Event
{
    Entity cameraEntity;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    CameraEvent(Entity cameraEntity, glm::mat4 viewMatrix, glm::mat4 projectionMatrix) 
        : cameraEntity(cameraEntity), viewMatrix(viewMatrix), projectionMatrix(projectionMatrix) {}
};