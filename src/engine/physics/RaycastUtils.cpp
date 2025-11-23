#include "RaycastUtils.h"
#include <algorithm>
#include <cfloat>
#include <cmath>

namespace Physics {
    RaycastHit raycastAABB(
        const glm::vec3& rayOrigin,
        const glm::vec3& rayDir,
        const glm::vec3& aabbMin,
        const glm::vec3& aabbMax)
    {
        RaycastHit result;

        float tMin = 0.0f;
        float tMax = FLT_MAX;

        for (int i = 0; i < 3; ++i) {
            if (std::abs(rayDir[i]) < 0.0001f) {
                // Ray is parallel to slab
                if (rayOrigin[i] < aabbMin[i] || rayOrigin[i] > aabbMax[i]) {
                    return result; // Miss
                }
            } else {
                float t1 = (aabbMin[i] - rayOrigin[i]) / rayDir[i];
                float t2 = (aabbMax[i] - rayOrigin[i]) / rayDir[i];

                if (t1 > t2) std::swap(t1, t2);

                tMin = std::max(tMin, t1);
                tMax = std::min(tMax, t2);

                if (tMin > tMax) {
                    return result; // Miss
                }
            }
        }

        if (tMax >= 0.0f) {
            result.hit = true;
            result.distance = tMin >= 0.0f ? tMin : tMax;
            result.point = rayOrigin + rayDir * result.distance;

            // Calculate which face was hit
            glm::vec3 center = (aabbMin + aabbMax) * 0.5f;
            glm::vec3 localHit = result.point - center;
            glm::vec3 size = (aabbMax - aabbMin) * 0.5f;

            float minDist = FLT_MAX;
            for (int i = 0; i < 3; ++i) {
                float dist = std::abs(std::abs(localHit[i]) - size[i]);
                if (dist < minDist) {
                    minDist = dist;
                    result.normal = glm::vec3(0.0f);
                    result.normal[i] = localHit[i] > 0 ? 1.0f : -1.0f;
                }
            }
        }

        return result;
    }

    RaycastHit raycastAABBWithCallbacks(
        const glm::vec3& rayOrigin,
        const glm::vec3& rayDir,
        const glm::vec3& aabbMin,
        const glm::vec3& aabbMax,
        OnHitCallback onHit,
        OnMissCallback onMiss)
    {
        RaycastHit result = raycastAABB(rayOrigin, rayDir, aabbMin, aabbMax);

        if (result.hit) {
            if (onHit) {
                onHit(result);
            }
        } else {
            if (onMiss) {
                onMiss(rayOrigin, rayDir);
            }
        }

        return result;
    }
}