#version 330 core
out vec4 FragColor;

in vec3 localPos;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);  // 1/(2*PI), 1/PI

vec2 SampleSphericalMap(vec3 viewDir) {
    vec2 uv = vec2(atan(viewDir.z, viewDir.x), asin(viewDir.y));
    uv *= invAtan; //scale angle in range of tan and sin
    uv += 0.5; // go from [-0.5,0.5] to [0.0,1.0]
    return uv;
}

void main() {
    vec2 uv = SampleSphericalMap(normalize(localPos));
    vec3 color = texture(equirectangularMap, uv).xyz;

    FragColor = vec4(color, 1.0);
}