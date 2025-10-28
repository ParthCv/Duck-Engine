#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Engine {
public:
    bool initialize(int width, int height);
    void run();
    void shutdown();

private:
    GLFWwindow* window = nullptr;
    int screenWidth = 1920;
    int screenHeight = 1080;

    void processInput();
    void update(float deltaTime);
    void render();
};