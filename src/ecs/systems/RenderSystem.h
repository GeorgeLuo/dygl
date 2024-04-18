#include "ColorComponent.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "GeometryComponent.h"
#include "ThreeDComponent.h" // Include the new ThreeDComponent header
#include "System.h"
#include "TransformComponent.h"
#include <glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include "ShaderManager.h"
#include "RenderComponent.h"
#include <glfw3.h>

void CheckGLError()
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error: " << err << std::endl;
        switch (err)
        {
        case GL_INVALID_ENUM:
            std::cerr << "GL_INVALID_ENUM" << std::endl;
            break;
        case GL_INVALID_VALUE:
            std::cerr << "GL_INVALID_VALUE" << std::endl;
            break;
        case GL_INVALID_OPERATION:
            std::cerr << "GL_INVALID_OPERATION" << std::endl;
            break;
        case GL_OUT_OF_MEMORY:
            std::cerr << "GL_OUT_OF_MEMORY" << std::endl;
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
            break;
        default:
            std::cerr << "Unknown error" << std::endl;
        }
    }
}

class RenderSystem : public System
{
public:
    RenderSystem();
    void Update(float dt, ComponentManager &componentManager);
    void Initialize();
    void RemoveEntity(Entity entity);

private:
    ShaderManager shaderManager;
    unsigned int geometryShaderProgram;
    unsigned int threeDShaderProgram;
    unsigned int shaderProgram;

    void setupShaderWithEntityData(TransformComponent &transform, float angle);

    void initializeShaders()
    {
        geometryShaderProgram = shaderManager.LoadShaderProgram(
            "shaders/vertex/basicTransform.vert",
            "shaders/fragment/uniformColor.frag");
        CheckGLError();

        threeDShaderProgram = shaderManager.LoadShaderProgram(
            "shaders/vertex/3DVertexShader.vert",
            "shaders/fragment/3DFragmentShader.frag");
        CheckGLError();
    }

    void setupGeometry(Entity entity, ComponentManager &componentManager)
    {
        if (!componentManager.HasComponent<RenderComponent>(entity))
        {
            unsigned int VAO, VBO;
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);
            glGenBuffers(1, &VBO);              // Generate VBO before using it
            glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind the buffer

            GLsizei numVertices = 0;
            if (componentManager.HasComponent<GeometryComponent>(entity))
            {
                auto &geometry = componentManager.GetComponent<GeometryComponent>(entity);
                numVertices = geometry.vertices.size();
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), geometry.vertices.data(), GL_STATIC_DRAW);
            }
            else if (componentManager.HasComponent<ThreeDComponent>(entity))
            {
                auto &threed = componentManager.GetComponent<ThreeDComponent>(entity);
                numVertices = threed.vertices.size();
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), threed.vertices.data(), GL_STATIC_DRAW);
            }
            glGenBuffers(1, &VBO);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
            glEnableVertexAttribArray(0);
            glBindVertexArray(0); // Unbind VAO to prevent further modifications.
            // Store the VAO and VBO in RenderComponent
            RenderComponent renderComponent{VAO, VBO};
            componentManager.AddComponent(entity, renderComponent);
            // Keep track that we've added the component to avoid duplication in future updates
        }
    }
};

RenderSystem::RenderSystem()
{
}

void RenderSystem::Initialize()
{
    initializeShaders();
    CheckGLError();

    glBindVertexArray(0); // Unbind the VAO to prevent unintended modifications.
}

void RenderSystem::Update(float dt, ComponentManager &componentManager)
{
    // Example light properties
    glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    // Current time in seconds (you might need to adjust this based on how you track time in your application)
    float currentTime = glfwGetTime(); // GLFW function, assuming GLFW is being used for window management
    float angle = currentTime;         // Simple rotation angle: rotate one full rotation per second

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glUseProgram(shaderProgram); // Use shader program
    CheckGLError();

    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f), // Camera is positioned at (0,0,3)
        glm::vec3(0.0f, 0.0f, 0.0f), // and looks towards the origin
        glm::vec3(0.0f, 1.0f,
                  0.0f) // Head is up (use y-axis as the 'up' direction)
    );

    // Position the camera to the side of the scene, looking at the origin
    // glm::vec3 cameraPos = glm::vec3(3.0f, 0.0f, 0.0f);    // Position to the side
    // glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); // Looking at the origin
    // glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);     // Up is along Y-axis

    // glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, upVector);

    // glm::mat4 projection =
    //     glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f,
    //                100.0f); // Example: Orthographic projection for simplicity.
    float zoomLevel = 2.0f; // Adjust this value to control zoom

    float windowWidth = 800;  // Example width
    float windowHeight = 600; // Example height
    float aspectRatio = windowWidth / windowHeight;

    // Choose to fit the view vertically, adjust horizontally

    glm::mat4 projection = glm::ortho(-zoomLevel * aspectRatio, zoomLevel * aspectRatio, -zoomLevel, zoomLevel, 0.1f, 100.0f);
    // glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

    // // Pass view and projection matrices to the shader
    // int viewLoc = glGetUniformLocation(shaderProgram, "view");
    // int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    // glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    // glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    for (auto entity : this->entities)
    {
        if (componentManager.HasComponent<GeometryComponent>(entity))
        {
            shaderProgram = geometryShaderProgram;
        }
        else if (componentManager.HasComponent<ThreeDComponent>(entity))
        {
            shaderProgram = threeDShaderProgram;
        }
        else
        {
            continue; // Skip entities without relevant components
        }

        glUseProgram(shaderProgram); // Use shader program

        int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

        int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

        // Pass view and projection matrices to the shader
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        if (!(componentManager.HasComponent<GeometryComponent>(entity) || componentManager.HasComponent<ThreeDComponent>(entity)))
            continue; // Skip entities without geometry or 3D components
        // Transform and setup geometry as before. The method now checks both component types.
        TransformComponent transform;
        if (componentManager.HasComponent<TransformComponent>(entity))
        {
            transform = componentManager.GetComponent<TransformComponent>(entity);
        }
        setupShaderWithEntityData(transform, angle);
        setupGeometry(entity, componentManager);
        // Check for ColorComponent; use default color if not available
        glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f}; // Default color: white
        if (componentManager.HasComponent<ColorComponent>(entity))
        {
            auto &colorComponent = componentManager.GetComponent<ColorComponent>(entity);
            color = glm::vec4(colorComponent.r, colorComponent.g, colorComponent.b, 1.0f);
        }

        // Pass color to the shader
        int colorLoc = glGetUniformLocation(shaderProgram, "ourColor");
        glUniform4fv(colorLoc, 1, glm::value_ptr(color));

        auto &renderComponent = componentManager.GetComponent<RenderComponent>(entity);
        glBindVertexArray(renderComponent.VAO);
        GLsizei numVertices = 0;
        if (componentManager.HasComponent<GeometryComponent>(entity))
        {
            auto &geometry = componentManager.GetComponent<GeometryComponent>(entity);
            numVertices = geometry.vertices.size();
        }
        else if (componentManager.HasComponent<ThreeDComponent>(entity))
        {
            auto &threed = componentManager.GetComponent<ThreeDComponent>(entity);
            numVertices = threed.vertices.size();
        }
        glDrawArrays(GL_TRIANGLES, 0, numVertices); // Use the appropriate number of vertices here
        CheckGLError();
    }
}

void RenderSystem::RemoveEntity(Entity entity)
{
    // Remove the entity from the system’s update list
    // entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
    entities.erase(entity);
}

void RenderSystem::setupShaderWithEntityData(TransformComponent &transform, float angle)
{
    glm::mat4 modelMatrix = transform.GetModelMatrix(); // Start with the original transform

    if (shaderProgram == threeDShaderProgram)
    {
        // Apply additional rotation to the model matrix based on the current time
        modelMatrix = glm::rotate(
            modelMatrix,                 // Start with the current model matrix
            glm::radians(angle * 50.0f), // Angle of rotation (radians per second)
            glm::vec3(0.0f, 1.0f, 0.0f)  // Axis of rotation (Y-axis here)
        );
    }

    // Update the shader uniform for the model matrix
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
}
