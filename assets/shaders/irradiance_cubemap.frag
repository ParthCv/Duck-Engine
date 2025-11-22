#version 330 core
out vec4 FragColor;
in vec3 localPos;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main() {
    vec3 N = normalize(localPos);
    vec3 irradiance = vec3(0.0);

    vec3 up = abs(N.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up = normalize(cross(N, right));

    float sampleDelta = 0.025;
    float nrSamples = 0.0;

    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta) {
            float cosTheta = cos(theta);
            float sinTheta = sin(theta);

            vec3 sampleVec = right * (sinTheta * cos(phi))
            + up * (sinTheta * sin(phi))
            + N * cosTheta;

            // I hv to fuking clamp HDR values to prevent extreme brightness - this too me 3 fuking hrs to figure this out
            vec3 sampleColor = min(texture(environmentMap, sampleVec).rgb, vec3(10.0));

            irradiance += sampleColor * cosTheta * sinTheta;
            nrSamples += 1.0;
        }
    }

    irradiance = PI * irradiance / nrSamples;

    FragColor = vec4(irradiance, 1.0);
}