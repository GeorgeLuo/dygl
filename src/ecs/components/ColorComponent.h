#pragma once

#include <algorithm>

struct ColorComponent
{
    float r, g, b;
    float targetR, targetG, targetB;
    float interpolationSpeed;

    bool dirty;

    ColorComponent(float r = 1.0f, float g = 1.0f, float b = 1.0f, float interpolationSpeed = 0.01f)
        : r(r), g(g), b(b), targetR(r), targetG(g), targetB(b), interpolationSpeed(interpolationSpeed) {}

    // Set new target color
    void SetTargetColor(float newR, float newG, float newB)
    {
        targetR = newR;
        targetG = newG;
        targetB = newB;
    }

    // Update the current color towards the target color
    void UpdateColor()
    {
        r = UpdateChannel(r, targetR);
        g = UpdateChannel(g, targetG);
        b = UpdateChannel(b, targetB);
    }

private:
    // Helper function to update a single color channel towards a target value
    float UpdateChannel(float current, float target)
    {
        if (current < target)
        {
            return std::min(current + interpolationSpeed, target);
        }
        else
        {
            return std::max(current - interpolationSpeed, target);
        }
    }
};