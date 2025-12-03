#pragma once

#include "glm/glm.hpp"
#include "../Entity.h"

struct RaycastSource {
    glm::vec3 direction{0.0f, 0.0f, -1.0f};
    float maxDistance = 100.0f;
    bool drawRay = true;

    bool lastHit = false;
    glm::vec3 lastHitPoint{};

    // ECS Pattern: Store the hit entity ID here so systems can process it later
    // Last min fix: Using EntityID instead of pointer to follow pure data component principle
    EntityID hitEntityID = INVALID_ENTITY_ID;
};
