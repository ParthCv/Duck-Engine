#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;

    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;

    Camera()
        : position(0.0f, 0.0f, 3.0f) // Hardcoded position for now
        , target(0.0f, 0.0f, 0.0f)
        , up(0.0f, 1.0f, 0.0f)
        , aspectRatio(16.0f / 9.0f)
        , fov(90.0f)
        , nearPlane(0.1f)
        , farPlane(100.0f)
    {}

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, target, up);
    }

    glm::mat4 getProjectionMatrix() const {
        return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }

    void updateAspectRatio(int width, int height) {
        aspectRatio = (float)width / (float)height;
    }
};