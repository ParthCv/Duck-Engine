//
// Created by Jeff on 2025-11-18.
//

#ifndef DUCKENGINE_RAYCASTUTILS_H
#define DUCKENGINE_RAYCASTUTILS_H

#pragma once
#include <glm/glm.hpp>
#pragma once
#include <glm/glm.hpp>
#include <functional>

namespace Physics {
    struct RaycastHit {
        bool hit;
        float distance;
        glm::vec3 point;
        glm::vec3 normal;
        void* userData;

        RaycastHit() : hit(false), distance(0.0f), point(0.0f), normal(0.0f), userData(nullptr) {}
    };

    using OnHitCallback = std::function<void(const RaycastHit&)>;
    using OnMissCallback = std::function<void(const glm::vec3& rayOrigin, const glm::vec3& rayDir)>;

    // Basic raycast using AABB
    RaycastHit raycastAABB(
        const glm::vec3& rayOrigin,
        const glm::vec3& rayDir,
        const glm::vec3& aabbMin,
        const glm::vec3& aabbMax
    );

    // Raycast with callbacks
    RaycastHit raycastAABBWithCallbacks(
        const glm::vec3& rayOrigin,
        const glm::vec3& rayDir,
        const glm::vec3& aabbMin,
        const glm::vec3& aabbMax,
        OnHitCallback onHit = nullptr,
        OnMissCallback onMiss = nullptr
    );
}
#endif //DUCKENGINE_RAYCASTUTILS_H