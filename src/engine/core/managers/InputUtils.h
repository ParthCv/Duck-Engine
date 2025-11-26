#pragma once
#include <glm/glm.hpp>
#include "../../renderer/Camera.h"

class InputUtils {
public:
    // Convert screen space mouse coordinates to normalized device coordinates (NDC)
    static glm::vec2 screenToNDC(glm::vec2 screenPos, int screenWidth, int screenHeight) {
        // Convert screen coords (top-left origin) to NDC (-1 to 1, center origin)
        float x = (2.0f * screenPos.x) / screenWidth - 1.0f;
        float y = 1.0f - (2.0f * screenPos.y) / screenHeight; // Flip Y axis
        return glm::vec2(x, y);
    }

    // Create a ray from camera through mouse position into world space
    // Useful for 3D picking/shooting in Duck Hunt
    static glm::vec3 screenToWorldRay(glm::vec2 screenPos, int screenWidth, int screenHeight,
                                       const Camera& camera) {
        // Convert to NDC
        glm::vec2 ndc = screenToNDC(screenPos, screenWidth, screenHeight);

        // Convert to clip space (add z and w)
        glm::vec4 clipCoords = glm::vec4(ndc.x, ndc.y, -1.0f, 1.0f);

        // Convert to eye/camera space
        glm::mat4 invProjection = glm::inverse(camera.getProjectionMatrix());
        glm::vec4 eyeCoords = invProjection * clipCoords;
        eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);

        // Convert to world space
        glm::mat4 invView = glm::inverse(camera.getViewMatrix());
        glm::vec4 worldCoords = invView * eyeCoords;
        glm::vec3 rayDirection = glm::vec3(worldCoords.x, worldCoords.y, worldCoords.z);

        // Normalize the ray direction
        return glm::normalize(rayDirection);
    }

    // Check if a ray intersects with a sphere (useful for duck hit detection)
    // Returns true if hit, and sets hitDistance to the distance along the ray
    static bool raySphereIntersection(glm::vec3 rayOrigin, glm::vec3 rayDirection,
                                      glm::vec3 sphereCenter, float sphereRadius,
                                      float& hitDistance) {
        glm::vec3 oc = rayOrigin - sphereCenter;
        float a = glm::dot(rayDirection, rayDirection);
        float b = 2.0f * glm::dot(oc, rayDirection);
        float c = glm::dot(oc, oc) - sphereRadius * sphereRadius;
        float discriminant = b * b - 4 * a * c;

        if (discriminant < 0) {
            return false; // No intersection
        }

        // Calculate nearest intersection point
        hitDistance = (-b - glm::sqrt(discriminant)) / (2.0f * a);
        return hitDistance > 0; // Only count hits in front of camera
    }

    // Check if a point is within a 2D rectangle (useful for UI buttons)
    static bool pointInRect(glm::vec2 point, glm::vec2 rectPos, glm::vec2 rectSize) {
        return point.x >= rectPos.x && point.x <= rectPos.x + rectSize.x &&
               point.y >= rectPos.y && point.y <= rectPos.y + rectSize.y;
    }
};