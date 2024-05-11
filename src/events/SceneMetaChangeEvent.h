#pragma once

struct SceneMetaChangeEvent : Event
{
    Entity entity;

    SceneMetaChangeEvent(Entity entity) : entity(entity) {}
};