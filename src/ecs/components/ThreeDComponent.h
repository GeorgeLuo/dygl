#pragma once
#include <vector>
#include <glm.hpp>
#include "Vertex.h"

// ThreeDComponent struct as per your instructions
struct ThreeDComponent
{
    ThreeDComponent() = default;

    std::vector<Vertex> vertices;
    ThreeDComponent(std::vector<Vertex> verts) : vertices(verts) {}
};