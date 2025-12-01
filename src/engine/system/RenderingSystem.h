#pragma once

class World;
class Shader;
class Camera;
class Material;

class RenderingSystem {
public:
    void renderEntities(World& world, Shader& shader, Camera& camera, Material& defaultMaterial);
};
