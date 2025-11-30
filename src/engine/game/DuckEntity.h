#ifndef DUCKENGINE_DUCKENTITY_H
#define DUCKENGINE_DUCKENTITY_H
#include "../ecs/Entity.h"
#include "../renderer/Material.h"
#include "glm/vec3.hpp"


class StaticMesh;

class DuckEntity : public Entity
{

    Material material;

public:
    float accumulatedTime = 0;
    float timeSinceDeath = 0;
    float pauseAfterKillDuration = 0.8f;

    glm::vec3 spawnPosition{};

    float escapeDistance = 100;

    bool isDead = false;
    bool isFalling = false;
    float DeathPlaneYBound = -10.0f;

    DuckEntity(World& InWorld);
    DuckEntity(World& InWorld, glm::vec3& InPosition);
    DuckEntity(World& InWorld, glm::vec3& InPosition, float speed);

    ~DuckEntity() override;

    void update(float deltaTime) override;

    void beginPlay() override;

    void setRandomFlightPath();

    void checkIfEscaped();

    void KillDuck();

    void fall();
    void cook();    // Change static mesh on death
    void createCube(StaticMesh& mesh);
};


#endif //DUCKENGINE_DUCKENTITY_H