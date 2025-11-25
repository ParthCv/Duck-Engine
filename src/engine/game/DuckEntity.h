#ifndef DUCKENGINE_DUCKENTITY_H
#define DUCKENGINE_DUCKENTITY_H
#include "../ecs/Entity.h"
#include "glm/vec3.hpp"


class DuckEntity : public Entity
{
public:
    float accumulatedTime = 0;

    glm::vec3 spawnPosition{};

    float escapeDistance = 20;

    bool isDead = false;
    float DeathPlaneYBound = -10.0f;

    DuckEntity(World& InWorld);

    DuckEntity(World& InWorld, glm::vec3& InPosition);

    ~DuckEntity() override;

    void update(float deltaTime) override;

    void beginPlay() override;

    void setRandomFlightPath();

    void checkIfEscaped();

    void KillDuck();
};


#endif //DUCKENGINE_DUCKENTITY_H