#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec2 gMetallicRoughness;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct MaterialData {
    // Texture maps
    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D aoMap;
    sampler2D metallicRoughnessMap;

    // Scalar fallbacks
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;

    // Flags
    bool hasAlbedoMap;
    bool hasNormalMap;
    bool hasMetallicMap;
    bool hasRoughnessMap;
    bool hasAOMap;
    bool hasMetallicRoughnessMap;
};

uniform MaterialData material;

void main() {
    gPosition = vec4(FragPos, 1.0);

    vec3 normal;
    if (material.hasNormalMap) {
        normal = texture(material.normalMap, TexCoords).rgb;
    } else {
        normal = normalize(Normal);
    }
    gNormal = vec4(normal, 1.0);


    vec3 albedo;
    if (material.hasAlbedoMap) {
        albedo = texture(material.albedoMap, TexCoords).xyz;
    } else {
        albedo = material.albedo;
    }

    float ao;
    if (material.hasAOMap) {
        ao = texture(material.aoMap, TexCoords).z;
    } else {
        ao = material.ao;
    }

    gAlbedo = vec4(albedo * ao, 1.0);

    float metallic;
    float roughness;

    if (material.hasMetallicRoughnessMap) {
        vec2 mr = texture(material.metallicRoughnessMap, TexCoords).yz;
        metallic = mr.x;
        roughness = mr.y;
    } else {
        if (material.hasMetallicMap) {
            metallic = texture(material.metallicMap, TexCoords).x;
        } else {
            metallic = material.metallic;
        }

        if (material.hasRoughnessMap) {
            roughness = texture(material.roughnessMap, TexCoords).x;
        } else {
            roughness = material.roughness;
        }
    }

    gMetallicRoughness = vec2(metallic, roughness);
}