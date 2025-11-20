#ifndef DUCKENGINE_DUCKENTITY_H
#define DUCKENGINE_DUCKENTITY_H
#include "../ecs/Entity.h"
#include "glm/vec3.hpp"


class DuckEntity : public Entity
{
public:
    float accumulatedTime = 0;

    DuckEntity(World& InWorld);

    DuckEntity(World& InWorld, glm::vec3& InPosition);

    ~DuckEntity() override;

    void Update(float deltaTime) override;

    void BeginPlay() override;
};


#endif //DUCKENGINE_DUCKENTITY_H