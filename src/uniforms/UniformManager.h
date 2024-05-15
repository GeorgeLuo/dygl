#pragma once
#include "UniformData.h"
#include "Entity.h"
#include "SceneContext.h"
#include "ComponentManager.h"
#include <unordered_map>
#include <vector>
#include <mutex>

class UniformManager
{
public:
    UniformManager(SceneContext &sceneContext, ComponentManager &componentManager);
    void StoreEntityUniforms(Entity entity, std::string uniformName, std::vector<float> uniform);
    void StoreEntityUniforms(Entity entity, std::string uniformName, std::vector<int> uniform);
    void StoreEntityUniforms(Entity entity, std::string uniformName, int integer);
    void StoreEntityUniforms(Entity entity, std::string uniformName, glm::mat4 matrix);
    UniformData GetUniforms(Entity entity);
    SceneContext GetSceneContext();
    void SetSceneContext(SceneContext &newSceneContext);

private:
    std::unordered_map<Entity, UniformData> entityUniformMap;
    SceneContext &sceneContext;
    ComponentManager &componentManager;
    std::mutex mutex;
};

UniformManager::UniformManager(SceneContext &sceneContext, ComponentManager &componentManager)
    : componentManager(componentManager), sceneContext(sceneContext)
{
}

void UniformManager::StoreEntityUniforms(Entity entity, std::string uniformName, glm::mat4 matrix)
{
    std::lock_guard<std::mutex> lock(mutex);
    entityUniformMap[entity].mat4Uniforms[uniformName] = matrix;
}

void UniformManager::StoreEntityUniforms(Entity entity, std::string uniformName, int integer)
{
    std::lock_guard<std::mutex> lock(mutex);
    entityUniformMap[entity].intUniforms[uniformName] = std::vector<int>{integer};
}

void UniformManager::StoreEntityUniforms(Entity entity, std::string uniformName, std::vector<float> uniform)
{
    std::lock_guard<std::mutex> lock(mutex);
    entityUniformMap[entity].floatVecUniforms[uniformName] = uniform;
}

void UniformManager::StoreEntityUniforms(Entity entity, std::string uniformName, std::vector<int> uniform)
{
    std::lock_guard<std::mutex> lock(mutex);
    entityUniformMap[entity].intUniforms[uniformName] = uniform;
}

UniformData UniformManager::GetUniforms(Entity entity)
{
    std::lock_guard<std::mutex> lock(mutex);
    return entityUniformMap.at(entity);
}

SceneContext UniformManager::GetSceneContext()
{
    std::lock_guard<std::mutex> lock(mutex);
    return sceneContext;
}

void UniformManager::SetSceneContext(SceneContext &newSceneContext)
{
    std::lock_guard<std::mutex> lock(mutex);
    sceneContext = newSceneContext;
}
