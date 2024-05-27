

#pragma once

struct BoundingBoxComponent
{
    float x;
    float y;
    float width;
    float height;

    BoundingBoxComponent(float x = 0.0f, float y = 0.0f, float width = 0.0f, float height = 0.0f)
        : x(x), y(y), width(width), height(height) {}
};
