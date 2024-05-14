#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <vector>
#include "Vertex.h"

struct GeometryComponent
{
  std::vector<Vertex> vertices;
  // Default constructor
  GeometryComponent() = default;
  
  // Constructor that initializes vertices
  GeometryComponent(const std::vector<Vertex> &verts) : vertices(verts) {}
  // Constructor that initializes vertices
  GeometryComponent(const std::vector<float> &verts) {}

};