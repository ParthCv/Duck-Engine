#include <iostream>
#include "LightManager.h"
#include "../src/engine/renderer/Shader.h"

void LightManager::addDirectionalLight(const DirectionalLight &directionalLight) {
    if (directionalLights.size() > MAX_DIR_LIGHTS) {
        std::cerr << "Warning broski: Max directional light reached (" << MAX_DIR_LIGHTS << ")" << std::endl;
        return;
    }
    directionalLights.push_back(directionalLight);
}

void LightManager::addPointLight(const PointLight &pointLight) {
    if (pointLights.size() > MAX_POINT_LIGHTS) {
        std::cerr << "Warning broski: Max point light reached (" << MAX_POINT_LIGHTS << ")" << std::endl;
        return;
    }
    pointLights.push_back(pointLight);
}

bool LightManager::removeDirectionalLight(size_t index) {
    if (index < directionalLights.size()) {
        directionalLights.erase(directionalLights.begin() + index);
        return true;
    }
    return false;
}

bool LightManager::removePointLight(size_t index) {
    if (index < pointLights.size()) {
        pointLights.erase(pointLights.begin() + index);
        return true;
    }
    return false;
}

void LightManager::removeAllLights() {
    directionalLights.clear();
    pointLights.clear();
}

void LightManager::removeAllPointLights() {
    pointLights.clear();
}

void LightManager::removeAllDirectionLight() {
    directionalLights.clear();
}

DirectionalLight & LightManager::getDirectionalLight(size_t index) {
    return directionalLights.at(index);
}

PointLight & LightManager::getPointLight(size_t index) {
    return pointLights.at(index);
}

void LightManager::uploadToShader(Shader &shader) const {
    int numDirLights = static_cast<int>(directionalLights.size());
    shader.setInt("numDirLights", numDirLights);

    for (int i = 0; i < numDirLights; ++i) {
        std::string prefix = "dirLights[" + std::to_string(i) + "].";
        const auto& light = directionalLights[i];

        shader.setVec3((prefix + "direction").c_str(), light.direction);
        shader.setVec3((prefix + "color").c_str(), light.color * light.intensity);
        shader.setBool((prefix + "enabled").c_str(), light.enabled);
    }

    int numPointLights = static_cast<int>(pointLights.size());
    shader.setInt("numPointLights", numPointLights);

    for (int i = 0; i < numPointLights; ++i) {
        std::string prefix = "pointLights[" + std::to_string(i) + "].";
        const auto& light = pointLights[i];

        shader.setVec3((prefix + "position").c_str(), light.position);
        shader.setVec3((prefix + "color").c_str(), light.color * light.intensity);
        shader.setFloat((prefix + "radius").c_str(), light.attenuationRadius);
        shader.setBool((prefix + "enabled").c_str(), light.enabled);
    }
}


