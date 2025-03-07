//
// Created by socki on 3/2/2025.
//

#ifndef Z2_WINDOWMANAGER_H
#define Z2_WINDOWMANAGER_H


#include "GLFW/glfw3.h"
#include "math/math.h"

namespace Zayn {

    struct Engine;
    struct WindowManager {
        GLFWwindow* glfwWindow;
        vec2 windowSize;
        const char* name;
    };


    void InitWindowManager(Engine* engine, vec2 windowSize, const char* windowName );
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);

} // Zayn

#endif //Z2_WINDOWMANAGER_H
