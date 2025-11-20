#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

Texture::Texture() :
 id(0),
 width(0),
 height(0),
 nrChannels(0) {

}

Texture::~Texture() {
  if (id != 0) {
      glDeleteTextures(1, &id);
  }
}

bool Texture::loadFromFile(const std::string& filePath, unsigned int textureSlot) {
    this->path = filePath;
    this->textureUnit = textureSlot;

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

    if (!data) {
        std::cerr << "Failed to load texture" << std::endl;
        return false;
    }

    GLenum format;
    if (nrChannels == 3) {
        format = GL_RGB;
    } else if (nrChannels == 4) {
        format = GL_RGBA;
    } else if (nrChannels == 1) {
        format = GL_RED;
    } else {
        std::cerr << "Unsupported number of channels" << std::endl;
        stbi_image_free(data);
        return false;
    }

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    std::cout << "Loaded texture: " << this->path << " (" << width << "x" << height << ", " << nrChannels << " channels)" << std::endl;
    return true;

}

bool Texture::loadHDR(const std::string& filePath, unsigned int textureSlot) {
    this->path = filePath;
    this->textureUnit = textureSlot;

    stbi_set_flip_vertically_on_load(true);
    float *data = stbi_loadf(filePath.c_str(), &width, &height, &nrChannels, 0);

    if (!data) {
        std::cerr << "Failed to load HDR" << std::endl;
        return false;
    }

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB16F,
        width,
        height,
        0,
        GL_RGB,
        GL_FLOAT,
        data
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    std::cout << "Loaded texture: " << this->path << " (" << width << "x" << height << ", " << nrChannels << " channels)" << std::endl;
    return true;
}

void Texture::generateBRDFLUT(Shader &brdfShader, int size) {
    width = size;
    height = size;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, size, size, 0, GL_RG, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint fbo, rbo;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);

    glViewport(0, 0, size, size);
    brdfShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint quadVAO, quadVBO;
    float quadVertices[] = {
        -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);

    std::cout << "Generated BRDF LUT (" << size << "x" << size << ")" << std::endl;
}

void Texture::bind() const {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unbind() const {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, 0);
}

