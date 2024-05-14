#pragma once

struct Vertex
{
    float x, y, z; // Position
    float u, v;    // Texture coordinates

    Vertex(float px, float py, float pz, float tu = 0.0f, float tv = 0.0f)
        : x(px), y(py), z(pz), u(tu), v(tv) {}
};