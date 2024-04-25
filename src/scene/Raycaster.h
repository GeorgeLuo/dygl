#pragma once

#include "SceneContext.h"
#include "Entity.h"
#include "ComponentManager.h"
#include "TransformComponent.h"
#include <iostream>
#include <gtc/type_ptr.hpp>

class Raycaster
{
public:
    Raycaster() {}

    glm::vec3 screenToWorld(double screenX, double screenY, int screenWidth, int screenHeight, glm::mat4 view, glm::mat4 projection)
    {
        float x = (2.0f * screenX) / screenWidth - 1.0f;
        float y = 1.0f - (2.0f * screenY) / screenHeight;
        float z = -1.0f; // Near plane (or far plane depending on use case)

        // Homogeneous coordinates in clip space
        glm::vec4 clipSpace(x, y, z, 1.0f);

        // Convert from clip space to view space
        glm::vec4 viewSpace = glm::inverse(projection) * clipSpace;

        viewSpace.z = -3.0f; // Convert to correct Z-axis for a directional vector
        viewSpace.w = 0.0f;  // Zero out W to treat it as a direction


        // Convert from view space to world space
        glm::vec3 worldSpace = glm::vec3(glm::inverse(view) * viewSpace);

        // Normalized world direction
        glm::vec3 normalizedWorldSpace = glm::normalize(worldSpace);

        return normalizedWorldSpace; // Return normalized direction
    }

    Entity raycast(const glm::vec3 &rayOrigin, const glm::vec3 &rayDirection, ComponentManager &componentManager)
    {
        Entity closestEntity = INVALID_ENTITY;
        float closestDistance = std::numeric_limits<float>::max();

        for (auto entity : componentManager.GetEntitiesWithComponent<TransformComponent>())
        {
            glm::vec3 entityPosition = componentManager.GetComponent<TransformComponent>(entity).position;

            // Define a bounding volume for the entity (e.g., a bounding sphere)
            float entityRadius = 1.0f; // Example radius; adjust as needed

            // Compute the distance to the ray's origin and the intersection point
            float distance;
            bool intersects = rayIntersectsSphere(rayOrigin, rayDirection, entityPosition, entityRadius, distance);

            if (intersects && distance < closestDistance)
            {
                closestDistance = distance;
                closestEntity = entity;
            }
        }

        return closestEntity;
    }

    // Function to check if a ray intersects with a sphere
    bool rayIntersectsSphere(const glm::vec3 &rayOrigin, const glm::vec3 &rayDirection, const glm::vec3 &sphereCenter, float sphereRadius, float &distance)
    {
        glm::vec3 oc = sphereCenter - rayOrigin;
        float a = glm::dot(rayDirection, rayDirection);
        float b = 2.0f * glm::dot(oc, rayDirection);
        float c = glm::dot(oc, oc) - sphereRadius * sphereRadius;
        float discriminant = b * b - 4 * a * c;

        if (glm::length(rayDirection) > 1.01f || glm::length(rayDirection) < 0.99f)
        {
            std::cerr << "Ray direction is not normalized." << std::endl;
        }

        if (sphereRadius <= 0 || std::isnan(sphereRadius))
        {
            std::cerr << "Sphere radius is invalid." << std::endl;
        }

        if (discriminant < 0)
        {
            return false; // No intersection
        }
        else
        {
            distance = (-b - glm::sqrt(discriminant)) / (2.0f * a);
            return true;
        }
    }
};