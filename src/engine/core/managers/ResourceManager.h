
#ifndef DUCKENGINE_RESOURCEMANAGER_H
#define DUCKENGINE_RESOURCEMANAGER_H
#include <string>
#include <unordered_map>
#include <memory>
#include "../model/StaticMesh.h"

class ResourceManager
{
public:
    // Enforcing singleton pattern
    static ResourceManager& Get();
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::shared_ptr<StaticMesh> GetStaticMesh(const std::string& fileName);
    void CollectGarbage();

private:
    ResourceManager() = default;

    std::unordered_map<std::string, std::shared_ptr<StaticMesh>> MeshCache;
};

#endif //DUCKENGINE_RESOURCEMANAGER_H
