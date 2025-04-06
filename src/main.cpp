#include <iostream>



#include <GLFW/glfw3.h>



#include "Engine.h"


#define MAX_FRAMES_IN_FLIGHT 2

int main() {


    Zayn::Engine engine = {};

    if (!glfwInit())
    {
        return -1;
    }


    Zayn::InitEngine(&engine);

    std::cout << "Hello, World!" << std::endl;

    // Main render loop
    while (!glfwWindowShouldClose(engine.windowManager.glfwWindow))
    {
        // Update all engine systems
        Zayn::UpdateEngine(&engine);
    }
    
    // Cleanup
    Zayn::ShutdownEngine(&engine);

    // Terminate GLFW
    glfwTerminate();
    
    return 0;


}
