#version 330 core
out vec4 fragColor;

in vec3 vertexColor;

void main() {
    FragColor = vec4(vertexColor, 1.0);
}