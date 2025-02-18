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
#include "FeedProcessorSystem.h"
#include "SystemLogger.h"
#include "GameStateSystem.h"

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
    SystemLogger logger;

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
    systemManager.AddSystem<GameStateSystem>(entityManager, componentManager);

    // input
    systemManager.AddSystem<MessageSystem>(entityManager, componentManager, queueCollection, eventBus);
    systemManager.AddSystem<MouseSystem>(entityManager, componentManager, context);
    systemManager.AddSystem<KeyboardInputSystem>(entityManager, componentManager, &logger);

    // business logic - modifications to entities triggered by inputs
    // systemManager.AddSystem<LabelingSystem>(componentManager, uniformManager);

    systemManager.AddSystem<TextOverlaySystem>(entityManager, componentManager);

    // render
    systemManager.AddSystem<RenderSystem>(context, uniformManager);
    systemManager.AddSystem<RenderPreprocessorSystem>(componentManager, uniformManager);

    // internals
    systemManager.AddSystem<FeedProcessorSystem>(entityManager, componentManager, &logger);
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
    systemManager.GetSystem<GameStateSystem>().Initialize();
    systemManager.GetSystem<TextOverlaySystem>().Initialize("fonts/Nanum-Gothic-Coding/NanumGothicCoding-Regular.ttf");
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

        float delta = 0.016f;

        systemManager.GetSystem<GameStateSystem>().Update(delta);

        // input type systems
        systemManager.GetSystem<MessageSystem>().Update(delta);
        // mouse system
        systemManager.GetSystem<MouseSystem>().Update(delta);
        // keyboard system
        systemManager.GetSystem<KeyboardInputSystem>().Update(delta);

        // input processing system? game logic system?

        systemManager.GetSystem<TextOverlaySystem>().Update(delta);

        // scene model computation type systems
        // text overlay
        // animations

        // visualization type systems
        systemManager.GetSystem<RenderPreprocessorSystem>()
            .Update(0.016f);
        systemManager.GetSystem<RenderSystem>().UpdateV4(0.016f, componentManager);

        systemManager.GetSystem<FeedProcessorSystem>().Update(0.016f);

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
        bool shiftPressed = mods & GLFW_MOD_SHIFT;
        bool ctrlPressed = mods & GLFW_MOD_CONTROL;
        bool altPressed = mods & GLFW_MOD_ALT;

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            app->systemManager.GetSystem<MouseSystem>().LeftPress(xpos, ypos, shiftPressed, ctrlPressed, altPressed);
            {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(2) << "clicked (" << xpos << ", " << ypos << ")";
                std::string formattedString = oss.str();
                app->logger.Log(formattedString, "log_input_logger");
            }
        }

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            app->systemManager.GetSystem<MouseSystem>().LeftRelease(xpos, ypos);
        }

        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            app->systemManager.GetSystem<MouseSystem>().RightPress(xpos, ypos, shiftPressed, ctrlPressed, altPressed);
            {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(2) << "right clicked (" << xpos << ", " << ypos << ")";
                std::string formattedString = oss.str();
                app->logger.Log(formattedString, "log_input_logger");
            }
        }
    }
}

void OpenGLApp::cursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
{
    OpenGLApp *app = static_cast<OpenGLApp *>(glfwGetWindowUserPointer(window));
    if (app)
    {
        app->systemManager.GetSystem<MouseSystem>().Move(xpos, ypos);
    }
}

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

            app->systemManager.GetSystem<KeyboardInputSystem>().KeyPress(key, shiftPressed, ctrlPressed, altPressed);

            switch (key)
            {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_BACKSPACE:
                character = "DELETE";
                break;
            default:
                // Map the key to a character, considering shift modifier for uppercase
                character = shiftPressed ? getShiftedChar(key) : getChar(key);
                if (character == "\n")
                {
                    character = "ENTER";
                }
                if (character == " ")
                {
                    character = "SPACEBAR";
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
            app->logger.Log(formattedString, "log_input_logger");
        }
        else if (action == GLFW_RELEASE)
        {
            // app->systemManager.GetSystem<KeyboardInputSystem>().KeyRelease(key, shiftPressed, ctrlPressed, altPressed);
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
