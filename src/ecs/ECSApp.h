#pragma once
#include "ComponentManager.h"
#include "EntityManager.h"
#include "RenderSystem.h"
#include <glad.h> // Include GLAD before GLFW.
#include <glfw3.h>
#include <iostream>
#include <tuple>
#include "QueueCollection.h"
#include "MessageSystem.h"
#include "EventBus.h"
#include "InputSystem.h"

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

    SceneContext context;

    OpenGLApp(QueueCollection &queueCollection)
        : queueCollection(queueCollection),
          entityManager(eventBus),
          context(SceneContext(800, 600, glm::vec3(0.0f, 0.0f, 3.0f))),
          renderSystem(eventBus, context),
          //   messageSystem(entityManager, componentManager, queueCollection, eventBus)
          inputSystem(entityManager, componentManager, context),
          messageSystem(entityManager, componentManager, queueCollection)
    {
    }

    void initialize()
    {
        // Register the static callback function
        glfwSetMouseButtonCallback(window, OpenGLApp::staticMouseButtonCallback);
    }

    static void staticMouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
    {
        OpenGLApp *app = static_cast<OpenGLApp *>(glfwGetWindowUserPointer(window)); // Get the instance
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);        // Get mouse position
            app->inputSystem.handleMouseClick(xpos, ypos); // Access the instance method
        }
    }

    void setupWindow()
    {
        // Set the window user pointer to 'this' for the static callback to reference
        glfwSetWindowUserPointer(window, this);
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
        setupWindow();
        initialize();

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
            renderSystem.Update(0.016f, componentManager);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        // Cleanup GLFW
        glfwDestroyWindow(window);
        glfwTerminate();
    }

private:
    GLFWwindow *window;
    InputSystem inputSystem;
};