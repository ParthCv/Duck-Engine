#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

class Camera {
public:
    // Camera Vectors
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;

    // Camera options
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;
    float mouseSensitivity;

    Camera()
        : position(0.0f, 0.0f, 3.0f)
        , worldUp(0.0f, 1.0f, 0.0f)
        , yaw(-90.0f)   // Pointing along -Z by default
        , pitch(0.0f)
        , front(0.0f, 0.0f, -1.0f)
        , aspectRatio(16.0f / 9.0f)
        , fov(45.0f)    // Standard FPS FOV usually 60-90, keeping 45 or 90
        , nearPlane(0.1f)
        , farPlane(100.0f)
        , mouseSensitivity(0.1f)
    {
        updateCameraVectors();
    }

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, position + front, up);
    }

    glm::mat4 getProjectionMatrix() const {
        return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }

    void updateAspectRatio(int width, int height) {
        aspectRatio = (float)width / (float)height;
    }

    // Process mouse movement for FPS look
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw   += xoffset;
        pitch -= yoffset; // Reversed: y-coordinates range from bottom to top

        // Constrain pitch so screen doesn't flip
        if (constrainPitch) {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }

        updateCameraVectors();
    }

private:
    void updateCameraVectors() {
        // Calculate the new Front vector
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);

        // Re-calculate the Right and Up vector
        right = glm::normalize(glm::cross(front, worldUp));
        up    = glm::normalize(glm::cross(right, front));
    }
};
