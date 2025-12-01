#pragma once

#include "glm/glm.hpp"

struct BoxCollider {
    glm::vec3 center{0.0f};
    glm::vec3 size{1.0f};
    bool isTrigger = false;
};
