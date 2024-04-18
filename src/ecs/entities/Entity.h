// src/ecs/Entity.h
#pragma once
#include <cstdint>

using Entity = uint32_t;
const Entity MAX_ENTITIES = 5000;
const Entity INVALID_ENTITY = static_cast<uint32_t>(-1); // Definition for INVALID_ENTITY