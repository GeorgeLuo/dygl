#pragma once
#include "UniformData.h"
#include "Entity.h"
#include "SceneContext.h"
#include "ComponentManager.h"
#include <unordered_map>
#include <vector>

class UniformManager
{
public:
    void StoreEntityUniforms(Entity entity, std::string uniformName, std::vector<float> uniform);
    void StoreEntityUniforms(Entity entity, std::string uniformName, glm::mat4 matrix);
    UniformData GetUniforms(Entity entity);
    UniformManager(SceneContext &sceneContext, ComponentManager &componentManager);
    std::unordered_map<Entity, UniformData> entityUniformMap;
    SceneContext &sceneContext;

private:
    ComponentManager &componentManager;
};

UniformManager::UniformManager(SceneContext &sceneContext, ComponentManager &componentManager) : componentManager(componentManager), sceneContext(sceneContext)
{
}

void UniformManager::StoreEntityUniforms(Entity entity, std::string uniformName, glm::mat4 matrix)
{
    entityUniformMap[entity].mat4Uniforms[uniformName] = matrix;
}

void UniformManager::StoreEntityUniforms(Entity entity, std::string uniformName, std::vector<float> uniform)
{
    entityUniformMap[entity].floatVecUniforms[uniformName] = uniform;
}

UniformData UniformManager::GetUniforms(Entity entity)
{
    return entityUniformMap.at(entity);
}