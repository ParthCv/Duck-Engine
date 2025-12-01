#pragma once

#include "glm/glm.hpp"

struct DebugDrawable {
    bool drawCollider = true;
    glm::vec3 colliderColor{1.0f, 0.0f, 0.0f}; // Red
};
