#version 330 core
out vec4 FragColor;

uniform float time;
uniform vec2 resolution;

void main() {
    vec2 uv = gl_FragCoord.xy / resolution;

    // Position in bottom-right corner (adjust these values to move it)
    uv -= vec2(0.9, 0.09);
    uv *= 8.0;  // Adjust this to change size croski kun

    uv.x *= resolution.x / resolution.y;

    float angle = atan(uv.y, uv.x);
    float radius = length(uv);

    float ring = smoothstep(0.28, 0.30, radius) * smoothstep(0.42, 0.40, radius);

    float arc = step(0.0, cos(angle * 3.0 - time * 5.0));

    float intensity = ring * arc;

    vec3 darkBlue = vec3(0.0, 1.0, 1.0);  // Adjust color here broski
    FragColor = vec4(darkBlue, intensity);
}