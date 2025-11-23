//
// Created by Nathan on 2025-11-22.
//

#pragma once
#include <memory>

#include "glad/glad.h"
#include "glm/fwd.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> // for glm::value_ptr if you need it
#include "../ecs/World.h"

#include "Shader.h"

class Entity;

class ShadowMap {
public:
    ShadowMap();
    bool initialize();

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

private:
    GLuint depthMapFBO;
    GLuint depthMap;
    GLuint shadowTexture;
    // int resolutionWidth = 1024;
    // int resolutionHeight = 1024;
    int resolutionWidth = 1920;
    int resolutionHeight = 1080;

    Shader simpleDepthShader;
    Shader debugShader;

    // Light Space Transforms
    float nearPlane = 1.0f, farPlane = 7.5f;
    glm::mat4 lightProjection;
    glm::mat4 lightView;
    glm::mat4 lightSpaceMatrix;
};
