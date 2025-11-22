#include "Entity.h"
#include "World.h"

Entity::~Entity() {
}

Entity::Entity(World& InWorld) : world(nullptr)
{
    world = &InWorld;
}

void Entity::update(float deltaTime) {
}

void Entity::beginPlay() {
}

bool Entity::getIsActive() const
{
    return bIsActive;
}

void Entity::destroy()
{
    bIsActive = false;
}