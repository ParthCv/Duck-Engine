//
// Created by User on 11/18/2025.
//

#ifndef DUCKENGINE_DUCKENTITY_H
#define DUCKENGINE_DUCKENTITY_H
#include "../ecs/Entity.h"


class DuckEntity : public Entity
{
public:
    float accumulatedTime = 0;

    DuckEntity(World& InWorld);

    ~DuckEntity() override;

    void Update(float deltaTime) override;

    void BeginPlay() override;

private:

};


#endif //DUCKENGINE_DUCKENTITY_H