//
// Created by Nathan on 2025-11-22.
//

#include "ShadowMap.h"

#include <iostream>
#include <ostream>
#include "../src/engine/ecs/Component.h"



struct StaticMeshComponent;
class Entity;

ShadowMap::ShadowMap() : depthMapFBO(0), shadowTexture(0) {
}

bool ShadowMap::initialize() {
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

    // Light Space Transform
    // Orthographic perspective
    lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 20.0f);

    // Above our scene looking down
    // TODO: Line up with actualy directional light
    lightView = glm::lookAt(
       glm::vec3(-5.0f, 10.0f, -5.0f),  // Light position
       glm::vec3(0.0f, 0.0f, 0.0f),      // Looking at world 0,0,0
       glm::vec3(0.0f, 1.0f, 0.0f)       // Up vector
   );
    lightSpaceMatrix = lightProjection * lightView;

    // Load shaders
    if (!simpleDepthShader.loadFromFiles("../assets/shaders/simpleDepthShader.vert", "../assets/shaders/emptyShader.frag")) {
        std::cerr << "Failed to load simple depth shader" << std::endl;
        return false;
    }

    if (!debugShader.loadFromFiles("../assets/shaders/debugDepth.vert", "../assets/shaders/debugDepth.frag")) {
        std::cerr << "Failed to load simple debug depth shader" << std::endl;
        return false;
    }
    return true;
}

void ShadowMap::render(World& world) {
    GLint lightSpaceMatrixLocation = glGetUniformLocation(simpleDepthShader.programID, "lightSpaceMatrix");

    simpleDepthShader.use();
    glUniformMatrix4fv(lightSpaceMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    glViewport(0, 0, resolutionWidth, resolutionHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    // glCullFace(GL_FRONT); // Cull backfaces to offset peter-panning
    renderScene(world);
    // glCullFace(GL_BACK);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::renderScene(World& world) {
    // TODO: We render every entity once. Once for shadow pas and once in engine.render
    // See if we really have to to this
    for (auto& entity : world.EntityManager.GetEntities())
    {
        if (entity->hasComponent<StaticMeshComponent>())
        {
            auto& staticMeshComponent = entity->getComponent<StaticMeshComponent>();

            // Getting the Model.
            glm::mat4 model = staticMeshComponent.getTransformMatrix();

            simpleDepthShader.setMat4("model", model);

            glBindVertexArray(staticMeshComponent.VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
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



