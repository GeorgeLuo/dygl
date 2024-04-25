#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <tuple>

class SceneContext
{
public:
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::vec3 cameraPosition;
    float windowWidth, windowHeight;

    SceneContext(float windowWidth, float windowHeight, glm::vec3 cameraPosition) : windowWidth(windowWidth), windowHeight(windowHeight), cameraPosition(cameraPosition)
    {
        initializeMatrices(windowWidth, windowHeight, cameraPosition); // Default initialization
    }

    std::tuple<glm::vec3, glm::vec3> getLightProperties() const
    {
        glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        return {lightPos, lightColor};
    }

    glm::mat4 getProjectionMatrix(float windowWidth, float windowHeight) const
    {
        // float aspectRatio = windowWidth / windowHeight;
        // float zoomLevel = 2.0f;
        // return glm::ortho(-zoomLevel * aspectRatio, zoomLevel * aspectRatio, -zoomLevel, zoomLevel, 0.1f, 100.0f);
        return glm::perspective(glm::radians(45.0f), windowWidth / windowHeight, 0.1f, 100.0f);
    }

    void initializeMatrices(float windowWidth, float windowHeight, glm::vec3 cameraPosition)
    {
        viewMatrix = glm::lookAt(
            cameraPosition,                                                // Camera is here
            glm::vec3(0.0f, 0.0f, 0.0f),                                   // and looks at the origin
            glm::vec3(0.0f, 1.0f, 0.0f));                                  // Head is up
        projectionMatrix = getProjectionMatrix(windowWidth, windowHeight); // Example initialization
    }

    void updateView(const glm::mat4 &newView)
    {
        viewMatrix = newView; // Update the view matrix if needed
    }

    void updateProjection(const glm::mat4 &newProjection)
    {
        projectionMatrix = newProjection; // Update the projection matrix if needed
    }
};
