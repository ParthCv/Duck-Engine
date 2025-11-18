//
// Created by Jeff on 2025-11-18.
//

#include "TestGame.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

TestGame::TestGame()
    : shootInterval(2.0f),
      shootTimer(0.0f),
      rayStart(0.0f),
      rayEnd(0.0f),
      hitCount(0),
      missCount(0),
      cubeMin(-0.5f, -0.5f, -0.5f),
      cubeMax(0.5f, 0.5f, 0.5f),
      rayHit(false)
{
    srand(static_cast<unsigned int>(time(nullptr)));
}

void TestGame::update(float deltaTime) {
    shootTimer += deltaTime;

    if (shootTimer >= shootInterval) {
        fireRandomRaycast();
        shootTimer = 0.0f;
    }
}

void TestGame::fireRandomRaycast() {
    float angle = ((rand() % 360) / 360.0f) * 2.0f * 3.14159f;
    float distance = 3.0f;
    glm::vec3 rayOrigin(
        cos(angle) * distance,
        ((rand() % 200) / 100.0f) - 1.0f,
        sin(angle) * distance
    );

    // Ray direction points toward cube
    glm::vec3 cubeCenter = (cubeMin + cubeMax) * 0.5f;
    glm::vec3 rayDirection = glm::normalize(cubeCenter - rayOrigin);

    rayDirection.x += ((rand() % 100) / 100.0f - 0.5f) * 0.5f;
    rayDirection.y += ((rand() % 100) / 100.0f - 0.5f) * 0.5f;
    rayDirection = glm::normalize(rayDirection);

    std::cout << "\nFiring Random Raycast" << std::endl;
    std::cout << "Origin: (" << rayOrigin.x << ", " << rayOrigin.y << ", " << rayOrigin.z << ")" << std::endl;
    std::cout << "Direction: (" << rayDirection.x << ", " << rayDirection.y << ", " << rayDirection.z << ")" << std::endl;

    // Perform raycast with callbacks
    Physics::raycastAABBWithCallbacks(
        rayOrigin,
        rayDirection,
        cubeMin,
        cubeMax,
        [this](const Physics::RaycastHit& hit) { onHit(hit); },
        [this, rayOrigin, rayDirection](const glm::vec3& o, const glm::vec3& d) { onMiss(o, d); }
    );
}

void TestGame::onHit(const Physics::RaycastHit& hit) {
    hitCount++;
    rayStart = hit.point - glm::normalize(hit.point) * hit.distance;
    rayEnd = hit.point;
    rayHit = true;

    std::cout << "✓ HIT! Distance: " << hit.distance << std::endl;
    std::cout << "  Hit Point: (" << hit.point.x << ", " << hit.point.y << ", " << hit.point.z << ")" << std::endl;
    std::cout << "  Hit Normal: (" << hit.normal.x << ", " << hit.normal.y << ", " << hit.normal.z << ")" << std::endl;
    std::cout << "  Score: " << hitCount << " hits / " << missCount << " misses" << std::endl;
}

void TestGame::onMiss(const glm::vec3& origin, const glm::vec3& direction) {
    missCount++;
    rayStart = origin;
    rayEnd = origin + direction * 10.0f;
    rayHit = false;

    std::cout << "MISS!" << std::endl;
    std::cout << "Score: " << hitCount << " hits / " << missCount << " misses" << std::endl;
}