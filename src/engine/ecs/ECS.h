#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <cassert>
#include <algorithm>

using EntityID = uint32_t;
const EntityID INVALID_ENTITY = UINT32_MAX;

inline uint32_t getNextComponentTypeID() {
    static uint32_t id = 0;
    return id++;
}

template<typename T>
uint32_t getComponentTypeID() {
    static uint32_t id = getNextComponentTypeID();
    return id;
}

// Base class for type-erased storage
class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void remove(EntityID entity) = 0;
};

// Stores components of type T
template<typename T>
class ComponentArray : public IComponentArray {
public:
    void add(EntityID entity, const T& component) {
        assert(entityToIndex.find(entity) == entityToIndex.end());

        entityToIndex[entity] = components.size();
        indexToEntity.push_back(entity);
        components.push_back(component);
    }

    T& get(EntityID entity) {
        return components[entityToIndex[entity]];
    }

    bool has(EntityID entity) {
        return entityToIndex.find(entity) != entityToIndex.end();
    }

    void remove(EntityID entity) override {
        if (!has(entity)) return;

        size_t indexToRemove = entityToIndex[entity];
        size_t lastIndex = components.size() - 1;

        // Swap with last
        components[indexToRemove] = components[lastIndex];
        EntityID lastEntity = indexToEntity[lastIndex];
        entityToIndex[lastEntity] = indexToRemove;
        indexToEntity[indexToRemove] = lastEntity;

        // Remove last
        components.pop_back();
        indexToEntity.pop_back();
        entityToIndex.erase(entity);
    }

    std::vector<T>& getAll() { return components; }
    std::vector<EntityID>& getEntities() { return indexToEntity; }

private:
    std::vector<T> components;
    std::vector<EntityID> indexToEntity;
    std::unordered_map<EntityID, size_t> entityToIndex;
};

// Manages entities and components
class Registry {
public:
    EntityID createEntity() {
        EntityID id = nextEntityID++;
        allEntities.push_back(id);
        return id;
    }

    void destroyEntity(EntityID entity) {
        for (auto& [typeID, array] : componentArrays) {
            array->remove(entity);
        }

        // Remove from allEntities
        auto it = std::find(allEntities.begin(), allEntities.end(), entity);
        if (it != allEntities.end()) {
            allEntities.erase(it);
        }
    }

    std::vector<EntityID>& getAllEntities() {
        return allEntities;
    }

    template<typename T>
    void addComponent(EntityID entity, const T& component) {
        getOrCreateArray<T>()->add(entity, component);
    }

    template<typename T>
    T& getComponent(EntityID entity) {
        return getOrCreateArray<T>()->get(entity);
    }

    template<typename T>
    bool hasComponent(EntityID entity) {
        return getOrCreateArray<T>()->has(entity);
    }

    template<typename T>
    void removeComponent(EntityID entity) {
        getOrCreateArray<T>()->remove(entity);
    }

    // Get all entities that have component T
    template<typename T>
    std::vector<EntityID>& getEntitiesWith() {
        return getOrCreateArray<T>()->getEntities();
    }

    // Get all components of type T
    template<typename T>
    std::vector<T>& getAllComponents() {
        return getOrCreateArray<T>()->getAll();
    }

private:
    template<typename T>
    ComponentArray<T>* getOrCreateArray() {
        uint32_t typeID = getComponentTypeID<T>();

        if (componentArrays.find(typeID) == componentArrays.end()) {
            componentArrays[typeID] = std::make_unique<ComponentArray<T>>();
        }

        return static_cast<ComponentArray<T>*>(componentArrays[typeID].get());
    }

    EntityID nextEntityID = 0;
    std::vector<EntityID> allEntities;
    std::unordered_map<uint32_t, std::unique_ptr<IComponentArray>> componentArrays;
};
