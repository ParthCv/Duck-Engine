#include "ResourceManager.h"
#include <iostream>
#include "../model/ImportedModel.h"

ResourceManager& ResourceManager::Get()
{
    static ResourceManager instance;
    return instance;
}

std::shared_ptr<StaticMesh> ResourceManager::GetStaticMesh(const std::string& fileName)
{
    // If mesh is already loaded
    auto it = MeshCache.find(fileName);
    if (it != MeshCache.end()) {
        return it->second;
    }

    std::cout << "[ResourceManager] Loading Mesh: " << fileName << std::endl;

    const std::string& filePath = "../assets/models/" + fileName;
    ImportedModel model(filePath);
    if (model.getNumVertices() <= 0) {
        std::cerr << "[ResourceManager] Failed to load model at: " << filePath << std::endl;
        return nullptr;
    }

    auto newMesh = std::make_shared<StaticMesh>();
    newMesh->loadFromImportedModel(model);
    MeshCache[fileName] = newMesh;
    return newMesh;
}

void ResourceManager::CollectGarbage()
{
    for (auto it = MeshCache.begin(); it != MeshCache.end(); ) {
        // If the use_count of a shared_ptr is 1, it means only the ResourceManager
        // holds this pointer and no entities are currently using it.
        if (it->second.use_count() == 1) {
            std::cout << "[ResourceManager] Unloading unused mesh: " << it->first << std::endl;
            it = MeshCache.erase(it);
        } else {
            ++it;
        }
    }
}