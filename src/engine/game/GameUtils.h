
#ifndef DUCKENGINE_GAMEUTILS_H
#define DUCKENGINE_GAMEUTILS_H
#include <iostream>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>

/**
 * Generates a ring of vectors around a center point with variance.
 *
 * @param center                The center position of the ring (world space).
 * @param radius                The radius of the ring.
 * @param numPoints             How many vectors to generate.
 * @param horizontalVariance    The amount of random horizontal offset to apply to points around the radius.
 * @param verticalVariance      The amount of random vertical offset to apply to points around the radius.
 * @return std::vector<glm::vec3> List of generated positions.
 */
inline std::vector<glm::vec3> GenerateRingPoints(glm::vec3 center, float radius, int numPoints,
                                          float horizontalVariance = 0.f, float verticalVariance = 0.f) {
    std::vector<glm::vec3> ringPoints;
    ringPoints.reserve(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        float circleProgress = static_cast<float>(i) / static_cast<float>(numPoints);
        float angle = circleProgress * glm::two_pi<float>();
        float variableRadius = radius + glm::linearRand(-horizontalVariance, horizontalVariance);
        float variableHeight = glm::linearRand(-verticalVariance, verticalVariance);
        float x = variableRadius * std::cos(angle);
        float z = variableRadius * std::sin(angle);
        glm::vec3 point = center + glm::vec3(x, variableHeight, z);
        ringPoints.push_back(point);
    }
    return ringPoints;
}

/**
 * Generates a half ring of vectors around a center point with variance.
 *
 * @param center                The center position of the half ring (world space).
 * @param radius                The radius of the half ring.
 * @param numPoints             How many vectors to generate.
 * @param horizontalVariance    The amount of random horizontal offset to apply to points around the radius.
 * @param verticalVariance      The amount of random vertical offset to apply to points around the radius.
 * @param degreesOffset         Where to start generating points, shifts the half circle.
 * @return std::vector<glm::vec3> List of generated positions.
 */
inline std::vector<glm::vec3> GenerateHalfRingPoints(glm::vec3 center, float radius, int numPoints,
                                              float horizontalVariance = 0.f, float verticalVariance = 0.f,
                                              float degreesOffset = 0.f) {
    std::vector<glm::vec3> ringPoints;
    ringPoints.reserve(numPoints);
    float radiansOffset = glm::radians(degreesOffset);
    for (int i = 0; i < numPoints; ++i) {
        float circleProgress = static_cast<float>(i) / static_cast<float>(numPoints);
        float angle = circleProgress * glm::pi<float>() + radiansOffset;
        float variableRadius = radius + glm::linearRand(-horizontalVariance, horizontalVariance);
        float variableHeight = glm::linearRand(-verticalVariance, verticalVariance);
        float x = variableRadius * std::cos(angle);
        float z = variableRadius * std::sin(angle);
        glm::vec3 point = center + glm::vec3(x, variableHeight, z);
        ringPoints.push_back(point);
    }
    return ringPoints;
}

#endif //DUCKENGINE_GAMEUTILS_H
