#pragma once
#include <string>
#include <glad/glad.h>


class Texture {
public:
    GLuint id; // ID to access the texture
    std::string path; // file path to load the texture
    int width, height, nrChannels; // We get this values from the stb_image load
    unsigned int textureUnit; // Texture slot

    Texture();
    ~Texture();

    bool loadFromFile(const std::string& filePath, unsigned int textureSlot);
    bool loadHDR(const std::string& filePath, unsigned int textureSlot);

    void bind() const;
    void unbind() const;
};