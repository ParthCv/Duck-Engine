#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class InputManager {
public:
    // Initialize with window reference and set up callbacks
    static void initialize(GLFWwindow* window);

    // Call this at the start of each frame to update state
    static void update();

    // Mouse position methods
    static glm::vec2 getMousePosition();
    static glm::vec2 getMouseDelta();

    // Reset delta (useful when switching states/locking cursor)
    static void resetMouseDelta();

    // Helper to lock/unlock cursor for FPS mode
    static void setCursorVisible(bool visible);

    // Mouse button methods
    static bool isMouseButtonDown(int button);
    static bool isMouseButtonPressed(int button);  // Just pressed this frame
    static bool isMouseButtonReleased(int button); // Just released this frame

    // Keyboard methods
    static bool isKeyDown(int key);
    static bool isKeyPressed(int key);  // Just pressed this frame
    static bool isKeyReleased(int key); // Just released this frame

    // Window dimensions (useful for normalizing mouse coords)
    static int getWindowWidth();
    static int getWindowHeight();

private:
    // GLFW callbacks
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void windowSizeCallback(GLFWwindow* window, int width, int height);

    // State storage
    static GLFWwindow* windowPtr;

    static glm::vec2 mousePosition;
    static glm::vec2 mousePreviousPosition;

    static bool mouseButtons[GLFW_MOUSE_BUTTON_LAST + 1];
    static bool mouseButtonsPrevious[GLFW_MOUSE_BUTTON_LAST + 1];

    static bool keys[GLFW_KEY_LAST + 1];
    static bool keysPrevious[GLFW_KEY_LAST + 1];

    static int windowWidth;
    static int windowHeight;
};
