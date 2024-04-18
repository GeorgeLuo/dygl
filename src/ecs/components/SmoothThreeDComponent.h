#pragma once
#include <vector>
#include <glm.hpp>
#include "Vertex.h"

struct ThreeDVertex
{
    glm::vec3 Position;
    glm::vec3 Normal;

    ThreeDVertex(const glm::vec3 &pos, const glm::vec3 &norm)
        : Position(pos), Normal(norm) {}
};

struct ThreeDComponent
{
    ThreeDComponent() = default;

    std::vector<ThreeDVertex> vertices;
    std::vector<unsigned int> indices;
    // Constructor, methods, etc.
};