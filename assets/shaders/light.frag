#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetallicRoughness;

uniform vec3 viewPos;

// Make sure to match these from light manager
#define MAX_POINT_LIGHTS 16
#define MAX_DIR_LIGHTS 1

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    bool enabled;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float radius;
    bool enabled;
};

uniform DirectionalLight dirLights[MAX_DIR_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int numDirLights;
uniform int numPointLights;

const float PI = 3.14159265359;

// PBR Functions
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Calculate lighting contribution from a single light
vec3 calculateLighting(vec3 L, vec3 radiance, vec3 N, vec3 V, vec3 F0, vec3 albedo, float metallic, float roughness) {
    vec3 H = normalize(V + L);

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

void main() {
    // Sample G-Buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 Albedo = texture(gAlbedo, TexCoords).rgb;
    vec2 MetallicRoughness = texture(gMetallicRoughness, TexCoords).rg;
    float Metallic = MetallicRoughness.r;
    float Roughness = MetallicRoughness.g;

    vec3 View = normalize(viewPos - FragPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, Albedo, Metallic);

    vec3 accumLight = vec3(0.0);

    // Directional lights
    for (int i = 0; i < numDirLights; i++) {
        if (!dirLights[i].enabled) continue;

        vec3 L = normalize(-dirLights[i].direction);
        vec3 radiance = dirLights[i].color;

        accumLight += calculateLighting(L, radiance, Normal, View, F0, Albedo, Metallic, Roughness);
    }

    // Point lights
    for (int i = 0; i < numPointLights; i++) {
        if (!pointLights[i].enabled) continue;

        vec3 L = normalize(pointLights[i].position - FragPos);
        float distance = length(pointLights[i].position - FragPos);

        float attenuation = 1.0 / (distance * distance);
        float falloff = clamp(1.0 - (distance / pointLights[i].radius), 0.0, 1.0);
        falloff = falloff * falloff;
        attenuation *= falloff;

        vec3 radiance = pointLights[i].color * attenuation;

        accumLight += calculateLighting(L, radiance, Normal, View, F0, Albedo, Metallic, Roughness);
    }

    // Ambient lighting
    vec3 ambient = vec3(0.02) * Albedo;
    vec3 color = ambient + accumLight;

    // Gamma correction
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}
