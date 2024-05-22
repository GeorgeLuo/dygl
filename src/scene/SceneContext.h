#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <tuple>

// TODO: make this thread-safe with mutex protections

class SceneContext
{
public:
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::vec3 cameraPosition;
    float windowWidth, windowHeight;

    SceneContext(float windowWidth, float windowHeight, glm::vec3 cameraPosition);

    std::tuple<glm::vec3, glm::vec3> getLightProperties() const;
    glm::mat4 getPerspectiveProjectionMatrix(float windowWidth, float windowHeight) const;
    glm::mat4 getOrthographicProjectionMatrix(float windowWidth, float windowHeight) const;

    void initializeMatrices(float windowWidth, float windowHeight, glm::vec3 cameraPosition);
    void updateView(const glm::mat4 &newView);
    void updateProjection(const glm::mat4 &newProjection);
};

SceneContext::SceneContext(float windowWidth, float windowHeight, glm::vec3 cameraPosition)
    : windowWidth(windowWidth), windowHeight(windowHeight), cameraPosition(cameraPosition)
{
    initializeMatrices(windowWidth, windowHeight, cameraPosition);
}

std::tuple<glm::vec3, glm::vec3> SceneContext::getLightProperties() const
{
    glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    return {lightPos, lightColor};
}

glm::mat4 SceneContext::getPerspectiveProjectionMatrix(float windowWidth = 800.0f, float windowHeight = 600.0f) const
{
    return glm::perspective(glm::radians(45.0f), windowWidth / windowHeight, 0.1f, 100.0f);
}

glm::mat4 SceneContext::getOrthographicProjectionMatrix(float windowWidth = 800.0f, float windowHeight = 600.0f) const
{
    // return glm::ortho(0.0f, windowWidth, windowHeight, 0.0f, -1.0f, 1.0f);
    // return glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    return glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
}

void SceneContext::initializeMatrices(float windowWidth, float windowHeight, glm::vec3 cameraPosition)
{
    viewMatrix = glm::lookAt(
        cameraPosition,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
    projectionMatrix = getPerspectiveProjectionMatrix(windowWidth, windowHeight);
}

void SceneContext::updateView(const glm::mat4 &newView)
{
    viewMatrix = newView;
}

void SceneContext::updateProjection(const glm::mat4 &newProjection)
{
    projectionMatrix = newProjection;
}
