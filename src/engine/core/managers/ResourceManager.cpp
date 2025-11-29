#include "ResourceManager.h"
#include <iostream>
#include "../model/ImportedModel.h"
#include "../src/engine/renderer/Material.h"

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

std::shared_ptr<Material> ResourceManager::GetMaterial(const std::string &materialName) {
    auto it = MaterialCache.find(materialName);
    if (it != MaterialCache.end()) {
        return it->second;
    }

    std::cout << "[ResourceManager] Loading Material: " << materialName << std::endl;

    auto newMaterial = std::make_shared<Material>();

    if (materialName == "duck") {
        newMaterial->loadAlbedoMap("../assets/textures/duck.png");
        newMaterial->setMetallic(0.0f);
        newMaterial->setRoughness(0.8f);
    } else if (materialName == "env") {
        newMaterial->loadAlbedoMap("../assets/textures/env.png");
        newMaterial->setMetallic(0.0f);
        newMaterial->setRoughness(0.8f);
    } else if (materialName == "gun") {
        newMaterial->loadAlbedoMap("../assets/textures/gun.png");
        newMaterial->setMetallic(0.0f);
        newMaterial->setRoughness(0.8f);
    } else if (materialName == "turkey") {
        newMaterial->setAlbedo(glm::vec3(0.99f, 0.82f, 0.09));
        newMaterial->setMetallic(1.0f);
        newMaterial->setRoughness(0.0f);
    }

    MaterialCache[materialName] = newMaterial;
    return newMaterial;
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

    for (auto it = MaterialCache.begin(); it != MaterialCache.end(); ) {
        if (it->second.use_count() == 1) {
            std::cout << "[ResourceManager] Unloading unused material: " << it->first << std::endl;
            it = MaterialCache.erase(it);
        } else {
            ++it;
        }
    }
}
