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

// Return char from keycode
const std::string getChar(int key)
{
    switch (key)
    {
    case GLFW_KEY_A:
        return "a";
    case GLFW_KEY_B:
        return "b";
    case GLFW_KEY_C:
        return "c";
    case GLFW_KEY_D:
        return "d";
    case GLFW_KEY_E:
        return "e";
    case GLFW_KEY_F:
        return "f";
    case GLFW_KEY_G:
        return "g";
    case GLFW_KEY_H:
        return "h";
    case GLFW_KEY_I:
        return "i";
    case GLFW_KEY_J:
        return "j";
    case GLFW_KEY_K:
        return "k";
    case GLFW_KEY_L:
        return "l";
    case GLFW_KEY_M:
        return "m";
    case GLFW_KEY_N:
        return "n";
    case GLFW_KEY_O:
        return "o";
    case GLFW_KEY_P:
        return "p";
    case GLFW_KEY_Q:
        return "q";
    case GLFW_KEY_R:
        return "r";
    case GLFW_KEY_S:
        return "s";
    case GLFW_KEY_T:
        return "t";
    case GLFW_KEY_U:
        return "u";
    case GLFW_KEY_V:
        return "v";
    case GLFW_KEY_W:
        return "w";
    case GLFW_KEY_X:
        return "x";
    case GLFW_KEY_Y:
        return "y";
    case GLFW_KEY_Z:
        return "z";
    case GLFW_KEY_0:
        return "0";
    case GLFW_KEY_1:
        return "1";
    case GLFW_KEY_2:
        return "2";
    case GLFW_KEY_3:
        return "3";
    case GLFW_KEY_4:
        return "4";
    case GLFW_KEY_5:
        return "5";
    case GLFW_KEY_6:
        return "6";
    case GLFW_KEY_7:
        return "7";
    case GLFW_KEY_8:
        return "8";
    case GLFW_KEY_9:
        return "9";
    case GLFW_KEY_SPACE:
        return " ";
    case GLFW_KEY_MINUS:
        return "-";
    case GLFW_KEY_EQUAL:
        return "=";
    case GLFW_KEY_LEFT_BRACKET:
        return "[";
    case GLFW_KEY_RIGHT_BRACKET:
        return "]";
    case GLFW_KEY_BACKSLASH:
        return "\\";
    case GLFW_KEY_SEMICOLON:
        return ";";
    case GLFW_KEY_APOSTROPHE:
        return "'";
    case GLFW_KEY_GRAVE_ACCENT:
        return "`";
    case GLFW_KEY_COMMA:
        return ",";
    case GLFW_KEY_PERIOD:
        return ".";
    case GLFW_KEY_SLASH:
        return "/";
    case GLFW_KEY_TAB:
        return "\t";
    case GLFW_KEY_ENTER:
        return "\n";
    default:
        return "";
    }
}

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
    systemManager.GetSystem<TextOverlaySystem>().Initialize();
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

void OpenGLApp::keypressCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    OpenGLApp *app = static_cast<OpenGLApp *>(glfwGetWindowUserPointer(window));
    if (app)
    {
        if (action == GLFW_PRESS)
        {
            switch (key)
            {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            default:
                app->systemManager.GetSystem<TextOverlaySystem>().InsertText(0.0f, 0.0f, 0.0f, getChar(key));
                break;
            }
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
