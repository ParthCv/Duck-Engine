//
// Created by User on 10/28/2025.
//

#include "config.h"

int main()
{
    std::cout << "Hello World!" << std::endl;

    GLFWwindow* Window;

    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW3" << std::endl;
        return -1;
    }

    Window = glfwCreateWindow(640, 480, "Hello World!", NULL, NULL);

    return 0;
}