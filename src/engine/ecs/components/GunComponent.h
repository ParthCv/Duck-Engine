#pragma once

#include "glm/glm.hpp"

struct GunComponent {
    // Recoil state
    float recoilOffset = 0.0f;      // Current backward recoil offset
    float recoilPitch = 0.0f;       // Current upward pitch rotation

    // Recoil settings
    float recoilRecoverySpeed = 8.0f;   // How fast recoil recovers (lower = slower, more noticeable)
    float recoilOffsetAmount = 0.5f;    // How much gun moves back when firing
    float recoilPitchAmount = 0.25f;    // How much gun rotates up when firing

    // Camera follow settings
    glm::vec3 cameraOffset = glm::vec3(0.25f, -0.25f, 0.5f);  // Right, Down, Forward
    float baseYawOffset = 180.0f;  // Default rotation offset (in degrees)
};
