#include "ShadowMap.h"

#include <iostream>
#include <ostream>
#include "../ecs/system/TransformSystem.h"
#include "../ecs/components/Transform.h"
#include "../core/model/StaticMesh.h"
#include "../ecs/components/StaticMeshComponent.h"
#include "../ecs/Entity.h"

ShadowMap::ShadowMap() : depthMapFBO(0), shadowTexture(0) {
}

bool ShadowMap::initialize(LightManager& lightManager) {
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        resolutionWidth, resolutionHeight, 0,
        GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    // Orthographic perspective
    lightProjection = glm::ortho(-orthoBounds, orthoBounds, -orthoBounds, orthoBounds,
                                    nearPlane, farPlane);
    updateLightSpaceTransform(lightManager);

    // Load shaders
    if (!simpleDepthShader.loadFromFiles("../assets/shaders/shadow_depth.vert", "../assets/shaders/shadow_depth.frag")) {
        std::cerr << "Failed to load simple depth shader" << std::endl;
        return false;
    }
    if (!debugShader.loadFromFiles("../assets/shaders/debug_depth.vert", "../assets/shaders/debug_depth.frag")) {
        std::cerr << "Failed to load simple debug depth shader" << std::endl;
        return false;
    }
    return true;
}

// Synchronizes with light manager (ex. moving Directional Light)
void ShadowMap::updateLightSpaceTransform(LightManager& lightManager) {
    // Directional light position is the opposite it's direction * scalar
    glm::vec3 directionalLightPosition;
    if (lightManager.getDirectionalLightCount() >= 1) {
        directionalLightPosition = glm::normalize(lightManager.getDirectionalLight(0).direction) * -directionLightPositionScalar;
    } else {
        directionalLightPosition = glm::normalize(glm::vec3(-1.0f, 2.0f, -1.0f)) * -directionLightPositionScalar;
    }
    lightView = glm::lookAt(
       directionalLightPosition,
       lookAtOrigin,
       glm::vec3(0.0f, 1.0f, 0.0f)       // Up vector
   );
    lightSpaceMatrix = lightProjection * lightView;
}

void ShadowMap::render(World& world) {
    GLint lightSpaceMatrixLocation = glGetUniformLocation(simpleDepthShader.programID, "lightSpaceMatrix");

    simpleDepthShader.use();
    glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    glViewport(0, 0, resolutionWidth, resolutionHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    renderScene(world);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::renderScene(World& world) {
    for (auto& entity : world.EntityManager.GetEntities())
    {
        if (entity->hasComponent<StaticMeshComponent>())
        {
            auto& staticMeshComponent = entity->getComponent<StaticMeshComponent>();

            auto& transform = entity->getComponent<Transform>();
            glm::mat4 model = TransformSystem::getTransformMatrix(transform);

            simpleDepthShader.setMat4("model", model);

            staticMeshComponent.Mesh->bind();
            staticMeshComponent.Mesh->draw();
        }
    }
}

// Debug method to draw shadow map to window
void ShadowMap::renderDepthMapToScreen() {
    // Bind default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use a simple shader that displays the depth texture
    debugShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    debugShader.setInt("depthMap", 0);

    // Render a fullscreen quad
    renderQuad();
}

void ShadowMap::renderQuad() {
    static unsigned int quadVAO = 0;
    static unsigned int quadVBO;

    if (quadVAO == 0) {
        // Fullscreen quad vertices
        // positions (x,y,z) + texCoords (u,v)
        float quadVertices[] = {
            // positions        // texCoords
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
             1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
             1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        };

        // Setup VAO and VBO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);

        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        // Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

        // TexCoord attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}