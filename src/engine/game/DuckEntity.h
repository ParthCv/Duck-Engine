#ifndef DUCKENGINE_DUCKENTITY_H
#define DUCKENGINE_DUCKENTITY_H
#include "../ecs/Entity.h"
#include "glm/vec3.hpp"


class DuckEntity : public Entity
{
public:
    float accumulatedTime = 0;

    glm::vec3 spawnPosition{};

    float escapeDistance = 10;

    DuckEntity(World& InWorld);

    DuckEntity(World& InWorld, glm::vec3& InPosition);

    ~DuckEntity() override;

    void update(float deltaTime) override;

    void beginPlay() override;

    void setRandomFlightPath();

    void checkIfEscaped();
};


#endif //DUCKENGINE_DUCKENTITY_H