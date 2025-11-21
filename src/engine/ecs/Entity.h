#pragma once
#include <array>
#include <bitset>
#include <cstddef>

#include "ComponentType.h"


class World;
/*
 * Define the maximum amount of components an Entity can have.
 */
constexpr std::size_t MAX_COMPONENTS = 32;

/*
 * A bitset is like an array, but 1 or 0.
 * bitset[0] = 1; entity has a component
 * Bit operation are fast and work well for checking things each frame.
 */
using ComponentBitSet = std::bitset<MAX_COMPONENTS>;

/*
 * Fixed-size array, where each index is a Component (represented by void* pointer)
 * The index corresponds to the ComponentTypeID.
 * We use a fixed number of components so there's no dynamic allocation.
 */
using ComponentArray = std::array<void*, MAX_COMPONENTS>;

class Entity
{
ComponentArray ComponentArray{};
ComponentBitSet ComponentBitSet{};

public:
    virtual ~Entity();

    World* world;
    bool bIsActive = true;

    Entity(World& InWorld);

    virtual void update(float deltaTime);

    virtual void beginPlay();

    bool getIsActive() const;

    /*
     * Marks this Entity for destruction.
     */
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

        // Add the new component to the Array
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