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

    int i = 1;
    while (true)
    {
        i++;

        Zayn::UpdateEngine(&engine);

    }

    return 0;





}
