#pragma once
#include <glm/glm.hpp>

struct DirectionalLight {
    glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);
    float intensity = 1.0f;
    bool enabled = true;

    DirectionalLight() = default;
    DirectionalLight(glm::vec3 direction, glm::vec3 color, float intensity = 1.0f)
        : direction(glm::normalize(direction)), color(color), intensity(intensity), enabled(true) {}

};

struct PointLight {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 color = glm::vec3(0.0f, 1.0f, 1.0f);
    float intensity = 1.0f;
    float attenuationRadius = 10.0f;
    bool enabled = true;

    PointLight() = default;
    PointLight(const glm::vec3 position, const glm::vec3 color, const float attenuationRadius,
        const float intensity = 1.0f)
        : position(position), color(color), intensity(intensity), attenuationRadius(attenuationRadius), enabled(true) {}
};