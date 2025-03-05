//
// Created by socki on 3/2/2025.
//

#include "WindowManager.h"
#include "Engine.h"

namespace Zayn {


    void InitWindowManager(Engine* engine, vec2 windowSize, const char* windowName)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        engine->windowManager.glfwWindow = glfwCreateWindow(windowSize.x, windowSize.y, windowName, NULL, NULL);

        glfwMakeContextCurrent(engine->windowManager.glfwWindow);
        glfwSetWindowUserPointer(engine->windowManager.glfwWindow, &engine->inputManager);
        glfwSetFramebufferSizeCallback(engine->windowManager.glfwWindow, framebuffer_size_callback);

    }


    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        //auto zaynMem = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
        //zaynMem->vulkan.vkFramebufferResized = true;
    }
} // Zayn