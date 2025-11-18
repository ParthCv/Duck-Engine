//
// Created by Jeff on 2025-11-18.
//

#ifndef DUCKENGINE_TESTGAME_H
#define DUCKENGINE_TESTGAME_H


#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "../physics/RaycastUtils.h"

class TestGame {
public:
    TestGame();

    void update(float deltaTime);

    // Getters for debug rendering
    const glm::vec3& getRayStart() const { return rayStart; }
    const glm::vec3& getRayEnd() const { return rayEnd; }

    int getHitCount() const { return hitCount; }
    int getMissCount() const { return missCount; }


private:
    void fireRandomRaycast();
    void onHit(const Physics::RaycastHit& hit);
    void onMiss(const glm::vec3& origin, const glm::vec3& direction);

    // Timing
    float shootInterval;
    float shootTimer;

    // Debug visualization
    glm::vec3 rayStart;
    glm::vec3 rayEnd;

    // Stats
    int hitCount;
    int missCount;

    // Cube AABB (hardcoded for now)
    glm::vec3 cubeMin;
    glm::vec3 cubeMax;
};


#endif //DUCKENGINE_TESTGAME_H