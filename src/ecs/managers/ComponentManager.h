#pragma once
#include <unordered_map>
#include <memory>
#include <typeindex>
#include "Entity.h"
#include <array>
#include <cassert>
#include <unordered_set>
#include <mutex>

class IComponentArray
{
public:
    virtual ~IComponentArray() = default;
    virtual void RemoveComponentIfExists(Entity entity) = 0; // Pure virtual function

    // Define virtual methods common to all component arrays if necessary
};

template <typename T>
class ComponentArray : public IComponentArray
{
private:
    // An array to store component instances, indexed by Entity ID
    std::array<T, MAX_ENTITIES> componentStorage;
    // A simple way to track which entities have a component of this type
    std::array<bool, MAX_ENTITIES> entityToComponentMapping{};
    // Track the number of components stored (optional, based on need)
    size_t componentCount = 0;

public:
    ComponentArray() = default; // Explicitly defaulting the constructor
    void AddComponent(Entity entity, T component)
    {
        assert(entity < MAX_ENTITIES && "Entity ID out of range.");
        assert(!entityToComponentMapping[entity] && "Component already exists for entity.");

        // Assign the component to the entity
        componentStorage[entity] = component;
        // Mark that this entity now has this component type
        entityToComponentMapping[entity] = true;
        ++componentCount;
    }

    // Optional: Method to remove a component from an entity
    void RemoveComponent(Entity entity)
    {
        assert(entity < MAX_ENTITIES && "Entity ID out of range.");
        assert(entityToComponentMapping[entity] && "Removing non-existent component.");

        // Mark that this entity no longer has this component type
        entityToComponentMapping[entity] = false;
        --componentCount;
    }

    void RemoveComponentIfExists(Entity entity) override
    {
        if (entity < MAX_ENTITIES && entityToComponentMapping[entity])
        {
            entityToComponentMapping[entity] = false;
            --componentCount;
            // Optionally handle additional cleanup, e.g., destructors, resetting to default values, etc.
        }
    }

    // Method to get a reference to an entity's component
    T &GetComponent(Entity entity)
    {
        assert(entity < MAX_ENTITIES && "Entity ID out of range.");
        assert(entityToComponentMapping[entity] && "Component does not exist for entity.");

        return componentStorage[entity];
    }

    // Check if an entity has this component type
    bool HasComponent(Entity entity) const
    {
        assert(entity < MAX_ENTITIES && "Entity ID out of range.");
        return entityToComponentMapping[entity];
    }
};

class ComponentManager
{
private:
    // Map from component type to its corresponding ComponentArray
    std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> componentArrays;
    std::unordered_map<std::type_index, std::unordered_set<Entity>> entitiesByComponentType;
    std::unordered_set<Entity> validEntities;
    std::mutex mutex;

    // Method to retrieve the ComponentArray for a specific component type
    template <typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray()
    {
        std::type_index typeIndex = std::type_index(typeid(T));
        if (componentArrays.find(typeIndex) == componentArrays.end())
        {
            // ComponentArray for this type doesn't exist yet, so create it
            auto newArray = std::make_shared<ComponentArray<T>>();
            componentArrays[typeIndex] = newArray;
        }
        return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeIndex]);
    }

public:
    // Add a component to an entity
    template <typename T>
    void AddComponent(Entity entity, T component)
    {
        std::lock_guard<std::mutex> lock(mutex);
        GetComponentArray<T>()->AddComponent(entity, component);
        entitiesByComponentType[std::type_index(typeid(T))].insert(entity);
        validEntities.insert(entity);
    }

    template <typename T>
    bool HasComponent(Entity entity)
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto componentArray = GetComponentArray<T>();
        if (componentArray)
        {
            return componentArray->HasComponent(entity);
        }
        return false;
    }

    template <typename T>
    T &GetComponent(Entity entity)
    {
        std::lock_guard<std::mutex> lock(mutex);
        // Get the component array for type T.
        auto componentArray = GetComponentArray<T>();
        // Use the component array to get the component for the entity.
        return componentArray->GetComponent(entity);
    }

    template <typename T>
    void RemoveComponent(Entity entity)
    {
        std::lock_guard<std::mutex> lock(mutex);
        auto componentArray = GetComponentArray<T>(); // Retrieve the appropriate ComponentArray
        if (componentArray)
        {
            componentArray->RemoveComponent(entity);                           // Call RemoveComponent on the ComponentArray
            entitiesByComponentType[std::type_index(typeid(T))].erase(entity); // Also remove the entity from the entitiesByComponentType mapping
        }
    }

    void RemoveAllComponents(Entity entity)
    {
        std::lock_guard<std::mutex> lock(mutex);
        // Iterate over all component types
        for (auto &entry : componentArrays)
        {
            auto &componentArray = entry.second;
            if (componentArray)
            {
                // Cast IComponentArray to the specific ComponentArray type is not straightforward
                // since we don't know T here, but we can create a method in IComponentArray to handle removal safely
                componentArray->RemoveComponentIfExists(entity);
            }
        }
        // Clean up entity tracking for all component types
        for (auto &entry : entitiesByComponentType)
        {
            entry.second.erase(entity);
        }
        validEntities.erase(entity);
    }

    template <typename T>
    const std::unordered_set<Entity> &GetEntitiesWithComponent()
    {
        std::lock_guard<std::mutex> lock(mutex);
        return entitiesByComponentType[std::type_index(typeid(T))];
    }

    bool IsValidEntity(Entity entity)
    {
        std::lock_guard<std::mutex> lock(mutex);
        return validEntities.find(entity) != validEntities.end();
    }

    std::mutex &getMutex() { return mutex; }
};
