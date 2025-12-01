#pragma once

#include "glm/glm.hpp"

struct BoundsComponent {
    glm::vec3 spawnPosition{};
    float escapeDistance = 100.0f;
};
