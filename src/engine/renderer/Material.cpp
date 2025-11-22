#include "Material.h"

Material::Material() = default;
Material::~Material() = default;

// NOTE: The material class is going to overwrite texture unit later dynamically

bool Material::loadAlbedoMap(const std::string &path) {
    albedoMap = std::make_unique<Texture>();
    if (!albedoMap->loadFromFile(path,0)) {
        albedoMap.reset();
        return false;
    }
    return true;
}

bool Material::loadNormalMap(const std::string &path) {
    normalMap = std::make_unique<Texture>();
    if (!normalMap->loadFromFile(path, 1)) {
        normalMap.reset();
        return false;
    }
    return true;
}

bool Material::loadMetallicMap(const std::string &path) {
    metallicMap = std::make_unique<Texture>();
    if (!metallicMap->loadFromFile(path, 2)) {
        metallicMap.reset();
        return false;
    }
    return true;
}

bool Material::loadRoughnessMap(const std::string &path) {
    roughnessMap = std::make_unique<Texture>();
    if (!roughnessMap->loadFromFile(path, 3)) {
        roughnessMap.reset();
        return false;
    }
    return true;
}

bool Material::loadAOMap(const std::string &path) {
    aoMap = std::make_unique<Texture>();
    if (!aoMap->loadFromFile(path, 4)) {
        aoMap.reset();
        return false;
    }
    return true;
}

bool Material::loadMetallicRoughnessMap(const std::string &path) {
    metallicRoughnessMap = std::make_unique<Texture>();
    if (!metallicRoughnessMap->loadFromFile(path, 5)) {
        metallicRoughnessMap.reset();
        return false;
    }
    return true;
}

void Material::setAlbedo(const glm::vec3 &color) {
    albedoValue = color;
}

void Material::setMetallic(float metallic) {
    metallicValue = metallic;
}

void Material::setRoughness(float roughness) {
    roughnessValue = roughness;
}

void Material::setAO(float ao) {
    aoValue = ao;
}

void Material::bind(Shader &shader, unsigned int startUnit) {
    unsigned int unit = startUnit;

    if (albedoMap) {
        albedoMap->textureUnit = unit;
        albedoMap->bind();
        shader.setInt("material.albedoMap", unit);
        shader.setBool("material.hasAlbedoMap", true);
    } else {
        shader.setBool("material.hasAlbedoMap", false);
        shader.setVec3("material.albedo", albedoValue);
    }
    unit++;

    // Normal
    if (normalMap) {
        normalMap->textureUnit = unit;
        normalMap->bind();
        shader.setInt("material.normalMap", unit);
        shader.setBool("material.hasNormalMap", true);
    } else {
        shader.setBool("material.hasNormalMap", false);
    }
    unit++;

    // Metallic-Roughness (combined or separate)
    if (metallicRoughnessMap) {
        metallicRoughnessMap->textureUnit = unit;
        metallicRoughnessMap->bind();
        shader.setInt("material.metallicRoughnessMap", unit);
        shader.setBool("material.hasMetallicRoughnessMap", true);
        shader.setBool("material.hasMetallicMap", false);
        shader.setBool("material.hasRoughnessMap", false);
        unit++;
    } else {
        shader.setBool("material.hasMetallicRoughnessMap", false);

        // Metallic
        if (metallicMap) {
            metallicMap->textureUnit = unit;
            metallicMap->bind();
            shader.setInt("material.metallicMap", unit);
            shader.setBool("material.hasMetallicMap", true);
        } else {
            shader.setBool("material.hasMetallicMap", false);
            shader.setFloat("material.metallic", metallicValue);
        }
        unit++;

        // Roughness
        if (roughnessMap) {
            roughnessMap->textureUnit = unit;
            roughnessMap->bind();
            shader.setInt("material.roughnessMap", unit);
            shader.setBool("material.hasRoughnessMap", true);
        } else {
            shader.setBool("material.hasRoughnessMap", false);
            shader.setFloat("material.roughness", roughnessValue);
        }
        unit++;
    }

    // AO
    if (aoMap) {
        aoMap->textureUnit = unit;
        aoMap->bind();
        shader.setInt("material.aoMap", unit);
        shader.setBool("material.hasAOMap", true);
    } else {
        shader.setBool("material.hasAOMap", false);
        shader.setFloat("material.ao", aoValue);
    }
}

void Material::unbind() {
    if (albedoMap) albedoMap->unbind();
    if (normalMap) normalMap->unbind();
    if (roughnessMap) roughnessMap->unbind();
    if (metallicMap) metallicMap->unbind();
    if (aoMap) aoMap->unbind();
    if (metallicRoughnessMap) metallicRoughnessMap->unbind();
}



