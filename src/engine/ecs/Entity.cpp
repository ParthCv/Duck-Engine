#include "Entity.h"
#include "World.h"

Entity::~Entity() {
}

Entity::Entity(World& InWorld) : world(nullptr)
{
    world = &InWorld;
}

void Entity::Update(float deltaTime) {
}

void Entity::BeginPlay() {
}

bool Entity::GetIsActive() const
{
    return bIsActive;
}

void Entity::Destroy()
{
    bIsActive = false;
}