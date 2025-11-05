#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "Texture.h"

struct Material {
    glm::vec3 albedo = glm::vec3(1.0f);
    std::shared_ptr<Texture> albedoMap = nullptr;

    float metallic = 0.0f;
    std::shared_ptr<Texture> metallicMap = nullptr;

    float roughness = 0.5f;
    std::shared_ptr<Texture> roughnessMap = nullptr;

    std::shared_ptr<Texture> normalMap = nullptr;

    Material() = default;

    bool hasAlbedoMap() const { return albedoMap != nullptr; }
    bool hasMetallicMap() const { return metallicMap != nullptr; }
    bool hasRoughnessMap() const { return roughnessMap != nullptr; }
    bool hasNormalMap() const { return normalMap != nullptr; }
};