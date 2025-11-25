#pragma once
#include "Light.h"
#include "../src/engine/renderer/Shader.h"

constexpr int MAX_POINT_LIGHTS = 16;
constexpr int MAX_DIR_LIGHTS = 1;

class LightManager {
public:
    void addDirectionalLight(const DirectionalLight& directionalLight);
    void addPointLight(const PointLight& pointLight);

    bool removeDirectionalLight(size_t index);
    bool removePointLight(size_t index);

    void removeAllLights();
    void removeAllPointLights();
    void removeAllDirectionLight();

    DirectionalLight& getDirectionalLight(size_t index);
    PointLight& getPointLight(size_t index);

    [[nodiscard]] size_t getPointLightCount() const {return pointLights.size();}
    [[nodiscard]] size_t getDirectionalLightCount() const {return directionalLights.size();}

    void uploadToShader(Shader& shader) const;
private:
    std::vector<DirectionalLight> directionalLights;
    std::vector<PointLight> pointLights;

};


