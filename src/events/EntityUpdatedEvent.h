#pragma once

#include "Entity.h"
#include "Event.h"

struct EntityUpdatedEvent : Event
{
    Entity entity;

    EntityUpdatedEvent(Entity entity) : entity(entity) {}
};