#pragma once
#include <glm/glm.hpp>
#include <memory>

#include "Shader.h"
#include "Texture.h"

class Material {
public:
    Material();
    ~Material();

    bool loadAlbedoMap(const std::string& path);
    bool loadNormalMap(const std::string& path);
    bool loadMetallicMap(const std::string& path);
    bool loadRoughnessMap(const std::string& path);
    bool loadAOMap(const std::string& path);
    bool loadMetallicRoughnessMap(const std::string& path);

    void setAlbedo(const glm::vec3& color);
    void setMetallic(float metallic);
    void setRoughness(float roughness);
    void setAO(float ao);

    void bind(Shader& shader, unsigned int startUnit = 0);
    void unbind();

private:
    std::unique_ptr<Texture> albedoMap;
    std::unique_ptr<Texture> normalMap;
    std::unique_ptr<Texture> metallicMap;
    std::unique_ptr<Texture> roughnessMap;
    std::unique_ptr<Texture> aoMap;
    std::unique_ptr<Texture> metallicRoughnessMap;

    glm::vec3 albedoValue = glm::vec3(1.0f);
    float metallicValue = 0.0f;
    float roughnessValue = 0.5f;
    float aoValue = 1.0f;
};