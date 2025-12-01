#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

struct Transform {
    glm::vec3 position{0.0f};
    glm::quat rotation{};
    glm::vec3 scale{1.0f};
    glm::vec3 oldPosition{0.0f};
};
