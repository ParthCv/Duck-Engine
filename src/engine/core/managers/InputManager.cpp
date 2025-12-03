#include "InputManager.h"
#include <cstring>
#include <iostream>

GLFWwindow* InputManager::windowPtr = nullptr;
glm::vec2 InputManager::mousePosition = glm::vec2(0.0f);
glm::vec2 InputManager::mousePreviousPosition = glm::vec2(0.0f);
bool InputManager::mouseButtons[GLFW_MOUSE_BUTTON_LAST + 1] = {false};
bool InputManager::mouseButtonsPrevious[GLFW_MOUSE_BUTTON_LAST + 1] = {false};
bool InputManager::keys[GLFW_KEY_LAST + 1] = {false};
bool InputManager::keysPrevious[GLFW_KEY_LAST + 1] = {false};
int InputManager::windowWidth = 0;
int InputManager::windowHeight = 0;

void InputManager::initialize(GLFWwindow* window) {
    std::cout << "[InputManager] Initializing (POLLING MODE)..." << std::endl;

    if (window == nullptr) {
        std::cerr << "[InputManager] ERROR: window is nullptr!" << std::endl;
        return;
    }

    windowPtr = window;

    // Get initial window size
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    std::cout << "[InputManager] Window size: " << windowWidth << "x" << windowHeight << std::endl;

    // Get initial mouse position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    mousePosition = glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));
    mousePreviousPosition = mousePosition;

    // Still set window size callback
    glfwSetWindowSizeCallback(window, windowSizeCallback);

    std::cout << "[InputManager] Initialized successfully (using direct polling)!" << std::endl;
}

void InputManager::update() {
    if (windowPtr == nullptr) {
        std::cerr << "[InputManager] ERROR: update() called but windowPtr is null!" << std::endl;
        return;
    }

    // Copy current state to previous state
    std::memcpy(mouseButtonsPrevious, mouseButtons, sizeof(mouseButtons));
    std::memcpy(keysPrevious, keys, sizeof(keys));
    mousePreviousPosition = mousePosition;

    // Poll commonly used keys
    keys[GLFW_KEY_SPACE] = (glfwGetKey(windowPtr, GLFW_KEY_SPACE) == GLFW_PRESS);
    keys[GLFW_KEY_ESCAPE] = (glfwGetKey(windowPtr, GLFW_KEY_ESCAPE) == GLFW_PRESS);
    keys[GLFW_KEY_ENTER] = (glfwGetKey(windowPtr, GLFW_KEY_ENTER) == GLFW_PRESS);
    keys[GLFW_KEY_BACKSPACE] = (glfwGetKey(windowPtr, GLFW_KEY_BACKSPACE) == GLFW_PRESS);

    // WASD
    keys[GLFW_KEY_W] = (glfwGetKey(windowPtr, GLFW_KEY_W) == GLFW_PRESS);
    keys[GLFW_KEY_A] = (glfwGetKey(windowPtr, GLFW_KEY_A) == GLFW_PRESS);
    keys[GLFW_KEY_S] = (glfwGetKey(windowPtr, GLFW_KEY_S) == GLFW_PRESS);
    keys[GLFW_KEY_D] = (glfwGetKey(windowPtr, GLFW_KEY_D) == GLFW_PRESS);

    // Arrow keys
    keys[GLFW_KEY_UP] = (glfwGetKey(windowPtr, GLFW_KEY_UP) == GLFW_PRESS);
    keys[GLFW_KEY_DOWN] = (glfwGetKey(windowPtr, GLFW_KEY_DOWN) == GLFW_PRESS);
    keys[GLFW_KEY_LEFT] = (glfwGetKey(windowPtr, GLFW_KEY_LEFT) == GLFW_PRESS);
    keys[GLFW_KEY_RIGHT] = (glfwGetKey(windowPtr, GLFW_KEY_RIGHT) == GLFW_PRESS);

    // Function keys
    keys[GLFW_KEY_F1] = (glfwGetKey(windowPtr, GLFW_KEY_F1) == GLFW_PRESS);
    keys[GLFW_KEY_F11] = (glfwGetKey(windowPtr, GLFW_KEY_F11) == GLFW_PRESS);

    // Game control keys
    keys[GLFW_KEY_P] = (glfwGetKey(windowPtr, GLFW_KEY_P) == GLFW_PRESS);
    keys[GLFW_KEY_R] = (glfwGetKey(windowPtr, GLFW_KEY_R) == GLFW_PRESS);
    keys[GLFW_KEY_Q] = (glfwGetKey(windowPtr, GLFW_KEY_Q) == GLFW_PRESS);
    keys[GLFW_KEY_E] = (glfwGetKey(windowPtr, GLFW_KEY_E) == GLFW_PRESS);
    keys[GLFW_KEY_M] = (glfwGetKey(windowPtr, GLFW_KEY_M) == GLFW_PRESS);
    keys[GLFW_KEY_O] = (glfwGetKey(windowPtr, GLFW_KEY_O) == GLFW_PRESS);
    keys[GLFW_KEY_G] = (glfwGetKey(windowPtr, GLFW_KEY_G) == GLFW_PRESS);
    keys[GLFW_KEY_T] = (glfwGetKey(windowPtr, GLFW_KEY_T) == GLFW_PRESS);
    keys[GLFW_KEY_C] = (glfwGetKey(windowPtr, GLFW_KEY_C) == GLFW_PRESS);


    // Number keys (for testing)
    keys[GLFW_KEY_1] = (glfwGetKey(windowPtr, GLFW_KEY_1) == GLFW_PRESS);
    keys[GLFW_KEY_2] = (glfwGetKey(windowPtr, GLFW_KEY_2) == GLFW_PRESS);
    keys[GLFW_KEY_3] = (glfwGetKey(windowPtr, GLFW_KEY_3) == GLFW_PRESS);

    // Tab and Shift (for UI navigation)
    keys[GLFW_KEY_TAB] = (glfwGetKey(windowPtr, GLFW_KEY_TAB) == GLFW_PRESS);
    keys[GLFW_KEY_LEFT_SHIFT] = (glfwGetKey(windowPtr, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
    keys[GLFW_KEY_LEFT_CONTROL] = (glfwGetKey(windowPtr, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);

    // Poll mouse buttons
    mouseButtons[GLFW_MOUSE_BUTTON_LEFT] = (glfwGetMouseButton(windowPtr, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
    mouseButtons[GLFW_MOUSE_BUTTON_RIGHT] = (glfwGetMouseButton(windowPtr, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
    mouseButtons[GLFW_MOUSE_BUTTON_MIDDLE] = (glfwGetMouseButton(windowPtr, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);

    // Poll mouse position
    double xpos, ypos;
    glfwGetCursorPos(windowPtr, &xpos, &ypos);
    mousePosition = glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));
}

glm::vec2 InputManager::getMousePosition() {
    return mousePosition;
}

glm::vec2 InputManager::getMouseDelta() {
    return mousePosition - mousePreviousPosition;
}

void InputManager::resetMouseDelta() {
    // Sync previous position to current so delta becomes 0 for the next frame
    mousePreviousPosition = mousePosition;
}

void InputManager::setCursorVisible(bool visible) {
    if (windowPtr) {
        if (visible) {
            glfwSetInputMode(windowPtr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}

bool InputManager::isMouseButtonDown(int button) {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) {
        return false;
    }
    return mouseButtons[button];
}

bool InputManager::isMouseButtonPressed(int button) {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) {
        return false;
    }
    return mouseButtons[button] && !mouseButtonsPrevious[button];
}

bool InputManager::isMouseButtonReleased(int button) {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) {
        return false;
    }
    return !mouseButtons[button] && mouseButtonsPrevious[button];
}

bool InputManager::isKeyDown(int key) {
    if (key < 0 || key > GLFW_KEY_LAST) {
        return false;
    }
    return keys[key];
}

bool InputManager::isKeyPressed(int key) {
    if (key < 0 || key > GLFW_KEY_LAST) {
        return false;
    }
    return keys[key] && !keysPrevious[key];
}

bool InputManager::isKeyReleased(int key) {
    if (key < 0 || key > GLFW_KEY_LAST) {
        return false;
    }
    return !keys[key] && keysPrevious[key];
}

int InputManager::getWindowWidth() {
    return windowWidth;
}

int InputManager::getWindowHeight() {
    return windowHeight;
}

// Only the window size callback
void InputManager::windowSizeCallback(GLFWwindow* window, int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

// Empty callback stubs (not used in polling mode)
void InputManager::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {}
void InputManager::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {}
void InputManager::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {}
