#version 330 core
out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox;

void main() {
    vec3 color = texture(skybox, texCoords).xyz;

    // Tone mapping (HDR -> LDR)
    color = color / (color + vec3(1.0));

    // Gamma correction
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color,1.0);
}