#pragma once

#include "ComponentManager.h"
#include "EntityManager.h"
#include "RenderSystem.h"
#include "SystemManager.h"
#include "EventBus.h"
#include "QueueCollection.h"
#include "MouseSystem.h"
#include "MessageSystem.h"
#include <glfw3.h>
#include <iostream>
#include "RenderPreprocessorSystem.h"
#include "TextOverlaySystem.h"
#include <sstream>
#include <iomanip>
#include "KeyboardInputSystem.h"

#pragma region ClassDeclaration

class OpenGLApp
{
public:
    OpenGLApp(QueueCollection &queueCollection);
    void Initialize();
    void Run();

    static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    static void staticMouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
    static void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos);
    static void keypressCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

private:
    void initialize();
    void setupWindow();

    EventBus eventBus;
    EntityManager entityManager;
    ComponentManager componentManager;
    SystemManager systemManager;

    QueueCollection &queueCollection;

    SceneContext context;
    UniformManager uniformManager;

    GLFWwindow *window;
};

#pragma endregion

#pragma region Constructor

OpenGLApp::OpenGLApp(QueueCollection &queueCollection)
    : queueCollection(queueCollection),
      entityManager(eventBus),
      context(SceneContext(800, 600, glm::vec3(0.0f, 0.0f, 5.0f))),
      uniformManager(context, componentManager),
      systemManager()
{
    systemManager.AddSystem<RenderSystem>(eventBus, context, uniformManager);
    systemManager.AddSystem<MessageSystem>(entityManager, componentManager, queueCollection);
    systemManager.AddSystem<MouseSystem>(eventBus, entityManager, componentManager, context);
    systemManager.AddSystem<RenderPreprocessorSystem>(eventBus, componentManager, uniformManager);
    systemManager.AddSystem<TextOverlaySystem>(entityManager, componentManager);
    systemManager.AddSystem<KeyboardInputSystem>(eventBus);
}

#pragma endregion

#pragma region InitializationMethods

void OpenGLApp::Initialize()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    window = glfwCreateWindow(800, 600, "Render System Usage", NULL, NULL);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    setupWindow();
    initialize();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        exit(-1);
    }
    systemManager.GetSystem<RenderSystem>().Initialize();
    systemManager.GetSystem<TextOverlaySystem>().Initialize("fonts/Nanum-Gothic-Coding/NanumGothicCoding-Regular.ttf");
    // systemManager.GetSystem<TextOverlaySystem>().AddTextBlock("free_type", 0.0f, 0.0f, 0.0f);
    systemManager.GetSystem<TextOverlaySystem>().AddListener(eventBus);
}

void OpenGLApp::Run()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        systemManager.GetSystem<MessageSystem>().Update(0.016f);
        // systemManager.GetSystem<RenderSystem>().UpdateV3(0.016f, componentManager);
        systemManager.GetSystem<RenderPreprocessorSystem>().Update(0.016f);
        systemManager.GetSystem<RenderSystem>().UpdateV4(0.016f, componentManager);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}

#pragma endregion

#pragma region Callbacks

void OpenGLApp::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void OpenGLApp::staticMouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    OpenGLApp *app = static_cast<OpenGLApp *>(glfwGetWindowUserPointer(window));
    if (app)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            app->systemManager.GetSystem<MouseSystem>().handleMouseClick(xpos, ypos);
            {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(2) << "clicked (" << xpos << ", " << ypos << ")";
                std::string formattedString = oss.str();
                app->eventBus.publish(DisplayTextEvent(formattedString, "input_logger", true, false, 1.0f, -1.9f, 0.0f));
            }
        }

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        {
            app->systemManager.GetSystem<MouseSystem>().handleMouseRelease();
        }
    }
}

void OpenGLApp::cursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
{
    OpenGLApp *app = static_cast<OpenGLApp *>(glfwGetWindowUserPointer(window));
    if (app)
    {
        app->systemManager.GetSystem<MouseSystem>().handleMouseMove(xpos, ypos);
    }
}

// void OpenGLApp::keypressCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
// {
//     OpenGLApp *app = static_cast<OpenGLApp *>(glfwGetWindowUserPointer(window));
//     if (app)
//     {
//         if (action == GLFW_PRESS)
//         {
//             switch (key)
//             {
//             case GLFW_KEY_ESCAPE:
//                 glfwSetWindowShouldClose(window, GL_TRUE);
//                 break;
//             default:
//                 app->systemManager.GetSystem<TextOverlaySystem>().InputChar(key, "free_type");
//                 {
//                     std::ostringstream oss;
//                     oss << std::fixed << std::setprecision(2) << "pressed (" << getChar(key) << ")";
//                     std::string formattedString = oss.str();
//                     app->eventBus.publish(DisplayTextEvent(formattedString, "input_logger", 1.0f, -1.9f, 0.0f));
//                     break;
//                 }
//             }
//         }
//     }
// }

void OpenGLApp::keypressCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    OpenGLApp *app = static_cast<OpenGLApp *>(glfwGetWindowUserPointer(window));
    if (app)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            // Check for modifier keys
            bool shiftPressed = mods & GLFW_MOD_SHIFT;
            bool ctrlPressed = mods & GLFW_MOD_CONTROL;
            bool altPressed = mods & GLFW_MOD_ALT;
            std::string character;
            switch (key)
            {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_BACKSPACE:
                app->systemManager.GetSystem<KeyboardInputSystem>().InputChar(DELETE, INT_MIN, shiftPressed, ctrlPressed, altPressed);
                character = "DELETE";
                break;
            default:
                app->systemManager.GetSystem<KeyboardInputSystem>().InputChar(CHARACTER, key, shiftPressed, ctrlPressed, altPressed);

                // Map the key to a character, considering shift modifier for uppercase
                character = shiftPressed ? getShiftedChar(key) : getChar(key);
                if (character == "\n")
                {
                    character = "ENTER";
                }
                break;
            }
            // Log the keypress event
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2)
                << "pressed (" << character << ")"
                << (shiftPressed ? " with Shift" : "")
                << (ctrlPressed ? " with Ctrl" : "")
                << (altPressed ? " with Alt" : "");

            std::string formattedString = oss.str();
            app->eventBus.publish(DisplayTextEvent(formattedString, "input_logger", true, false, 0.5f, -1.9f, 0.0f));
        }
    }
}

#pragma endregion

#pragma region PrivateMethods

void OpenGLApp::initialize()
{
    glfwSetMouseButtonCallback(window, OpenGLApp::staticMouseButtonCallback);
    glfwSetCursorPosCallback(window, OpenGLApp::cursorPositionCallback);
    glfwSetKeyCallback(window, OpenGLApp::keypressCallback);
}

void OpenGLApp::setupWindow()
{
    glfwSetWindowUserPointer(window, this);
}

#pragma endregion
