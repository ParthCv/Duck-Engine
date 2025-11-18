//
// Created by super on 2025-11-03.
//

#include "Entity.h"

#include "World.h"

Entity::Entity(World& InWorld) : world(nullptr)
{
    world = &InWorld;
}

bool Entity::GetIsActive() const
{
    return bIsActive;
}

void Entity::Destroy()
{
    bIsActive = false;
}