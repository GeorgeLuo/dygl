#pragma once
#include <set>
#include "Entity.h"

class System
{
public:
    std::set<Entity> entities;
    virtual ~System() {}
    virtual void Update(float deltaTime) {}
    void AddEntity(Entity entity)
    {
        this->entities.insert(entity);
    }
    void RemoveEntity(Entity entity)
    {
        this->entities.erase(entity);
    }
};
