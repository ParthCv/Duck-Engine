#pragma once

struct HealthComponent {
    bool isDead = false;
    bool isFalling = false;
    float timeSinceDeath = 0.0f;
    float pauseAfterKillDuration = 0.8f;
    float DeathPlaneYBound = -10.0f;
};
