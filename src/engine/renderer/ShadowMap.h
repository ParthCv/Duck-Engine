#pragma once

#include "glad/glad.h"
#include "glm/fwd.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> // for glm::value_ptr if you need it
#include "../ecs/World.h"

#include "Shader.h"

class ShadowMap {
public:
    ShadowMap();
    bool initialize(LightManager& lightManagerRef);

    void render(World &world);

    void renderScene(World& world);

    void renderDepthMapToScreen();

    void renderQuad();

    GLuint& getDepthMap() {
        return depthMap;
    }
    glm::mat4& getLightSpaceMatrix() {
        return lightSpaceMatrix;
    }

    void updateLightSpaceTransform(LightManager& lightManager);

private:
    /*
     * If shadow clipping occurs:
     * - increase orthoBounds (rectangular area where shadows will be rendered, centered at lookAtOrigin)
     * - move lookAtOrigin (Closer to release, consider moving to somewhere in front of camera)
     * - increase farPlane (depth, how far away from light position shadows will render from)
    */
    float orthoBounds = 30.0f;
    float nearPlane = 0.1f, farPlane = 30.0f;
    glm::vec3 lookAtOrigin = glm::vec3(0.0f, 0.0f, 0.0f); // World origin
    float directionLightPositionScalar = 20.0f;   // High in the sky
    
    GLuint depthMapFBO;
    GLuint depthMap;
    GLuint shadowTexture;
    int resolutionWidth = 1920;
    int resolutionHeight = 1080;

    Shader simpleDepthShader;
    Shader debugShader;

    // Light Space Transforms
    glm::mat4 lightProjection;
    glm::mat4 lightView;
    glm::mat4 lightSpaceMatrix;
};
