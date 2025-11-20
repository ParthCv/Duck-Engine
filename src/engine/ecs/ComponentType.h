#ifndef DUCKENGINE_COMPONENTTYPE_H
#define DUCKENGINE_COMPONENTTYPE_H
#include <cstddef>

using ComponentTypeID = std::size_t;

inline ComponentTypeID GetComponentTypeID()
{
    static ComponentTypeID id = 0;
    return id++;
}

template <typename T>
ComponentTypeID GetComponentTypeID()
{
    static ComponentTypeID TypeID = GetComponentTypeID();
    return TypeID;
}

#endif //DUCKENGINE_COMPONENTTYPE_H