#pragma once
#include <glad/glad.h>
#include "Texture.h"
#include <memory>

class GBuffer {
public:
    GBuffer();
    ~GBuffer();

    bool initialize(int width, int height);
    void bindForWriting();
    void bindForReading();
    void unbind();
    void resize(int width, int height);
    GLuint getPositionTexture() const { return gPosition; }
    GLuint getNormalTexture() const { return gNormal; }
    GLuint getAlbedoTexture() const { return gAlbedo; }
    GLuint getMetallicRoughnessTexture() const { return gMetallicRoughness; }
    GLuint getDepthTexture() const { return depthBuffer; }

private:
    GLuint fbo;
    GLuint gPosition;          // RGB: Position, A: unused
    GLuint gNormal;            // RGB: Normal, A: unused
    GLuint gAlbedo;            // RGB: Albedo, A: unused
    GLuint gMetallicRoughness; // R: Metallic, G: Roughness, BA: unused
    GLuint depthBuffer;        // Depth renderbuffer

    int width, height;

    void cleanup();
};