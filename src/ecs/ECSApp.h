#pragma once
#include "ComponentManager.h"
#include "ConcurrentQueue.h"
#include "EntityManager.h"
#include "RenderSystem.h"
#include <glad.h> // Include GLAD before GLFW.
#include <glfw3.h>
#include <iostream>
#include <tuple>
#include "ColorComponent.h"
#include "TagComponent.h"
#include "QueueCollection.h"
#include "IdComponent.h"
#include "ChangeColorCommand.h"
#include "ThreeDComponent.h"
#include "MessageSystem.h"
#include "EventBus.h"

class OpenGLApp
{
public:
    // event bus
    EventBus eventBus;

    // managers
    EntityManager entityManager;
    ComponentManager componentManager;

    // systems
    RenderSystem renderSystem;
    MessageSystem messageSystem;

    QueueCollection &queueCollection;

    OpenGLApp(QueueCollection &queueCollection)
        : queueCollection(queueCollection),
          entityManager(eventBus),
          renderSystem(eventBus),
          messageSystem(entityManager, componentManager, queueCollection, eventBus)
    {
    }

    static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    void Initialize()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For macOS
#endif
        window = glfwCreateWindow(800, 600, "Render System Usage", NULL, NULL);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        if (window == NULL)
        {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            exit(-1);
        }
        glfwMakeContextCurrent(window);

        // GLAD: load all OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            exit(-1);
        }
        renderSystem.Initialize();
    }

    void Run()
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        // Rendering loop
        while (!glfwWindowShouldClose(window))
        {
            glClear(GL_COLOR_BUFFER_BIT);

            messageSystem.Update(0.016f);

            // Update the RenderSystem - this will handle all rendering for entities it knows about
            renderSystem.Update(0.016f, componentManager); // For this example, assuming a fixed timestep

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        // Cleanup GLFW
        glfwDestroyWindow(window);
        glfwTerminate();
    }

private:
    GLFWwindow *window;
};