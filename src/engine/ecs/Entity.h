//
// Created by super on 2025-11-03.
//

#ifndef DUCKENGINE_ENTITY_H
#define DUCKENGINE_ENTITY_H
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
private:
    ComponentArray ComponentArray{};
    ComponentBitSet ComponentBitSet{};

public:
    World* world;
    bool bIsActive = true;

    Entity(World& InWorld);

    bool GetIsActive() const;

    /*
     * Marks this Entity for destruction.
     */
    void Destroy();

    template<typename T>
    bool HasComponent()
    {
        return ComponentArray[GetComponentTypeID<T>()];
    }

    template<typename T, typename... nArgs>
    T& AddComponent(nArgs&&... args)
    {
        // Implements Perfect Fowarding; everything is not treated as a l-value. More efficient with moves and temporaries.
        T* component(new T(std::forward<nArgs>(args)...));

        // Add the new component to the Array
        ComponentArray[GetComponentTypeID<T>()] = component;
        ComponentBitSet[GetComponentTypeID<T>()] = true;

        return *component;
    }

    template<typename T>
    T& GetComponent() {
        auto ptr(ComponentArray[GetComponentTypeID<T>()]);
        return *static_cast<T*>(ptr);
    }

    template<typename T>
    void deactivateComponent() {
        ComponentBitSet[GetComponentTypeID<T>()] = false;
    }

};

#endif //DUCKENGINE_ENTITY_H