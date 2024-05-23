#pragma once
#include <set>
#include <memory>
#include "Entity.h"
#include "SystemLogger.h"

class System
{
public:
    std::set<Entity> entities;
    System(SystemLogger *logger = nullptr) : logger(logger) {}
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

    void Log(const std::string &message, std::string blockname)
    {
        if (logger)
        {
            logger->Log(message, blockname);
        }
    }

    SystemLogger *logger;
};
