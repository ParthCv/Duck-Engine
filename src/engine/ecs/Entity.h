#pragma once
#include <array>
#include <bitset>
#include <cstddef>
#include <cstdint>

#include "ComponentType.h"


class World;


using EntityID = std::uint64_t;
constexpr EntityID INVALID_ENTITY_ID = 0;

//maximum amount of components an Entity can have.

constexpr std::size_t MAX_COMPONENTS = 32;

using ComponentBitSet = std::bitset<MAX_COMPONENTS>;

// Each index is a Component , The index corresponds to the ComponentTypeID.
using ComponentArray = std::array<void*, MAX_COMPONENTS>;

class Entity
{
ComponentArray ComponentArray{};
ComponentBitSet ComponentBitSet{};

EntityID entityID = INVALID_ENTITY_ID;

public:
    virtual ~Entity();

    World* world;
    bool bIsActive = true;

    Entity(World& InWorld);

    EntityID getID() const { return entityID; }

    virtual void update(float deltaTime);

    virtual void beginPlay();

    bool getIsActive() const;

    void destroy();

    template<typename T>
    bool hasComponent()
    {
        return ComponentArray[getComponentTypeID<T>()];
    }

    template<typename T, typename... nArgs>
    T& addComponent(nArgs&&... args)
    {
        // Implements Perfect Fowarding; everything is not treated as a l-value. More efficient with moves and temporaries.
        T* component(new T(std::forward<nArgs>(args)...));

        ComponentArray[getComponentTypeID<T>()] = component;
        ComponentBitSet[getComponentTypeID<T>()] = true;

        return *component;
    }

    template<typename T>
    T& getComponent() {
        auto ptr(ComponentArray[getComponentTypeID<T>()]);
        return *static_cast<T*>(ptr);
    }

    template<typename T>
    void deactivateComponent() {
        ComponentBitSet[getComponentTypeID<T>()] = false;
    }
};