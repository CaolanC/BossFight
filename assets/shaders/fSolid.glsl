#version 330 core

in vec3 vWorldPos;
in vec3 vCamPos;

out vec4 FragColor;

void main() {
    float dist = length(vCamPos - vWorldPos);
    float darken = clamp(1.0 - dist * 0.03, 0.5, 1.0);
    vec3 base = vec3(0.9, 0.5, 0.7);
    FragColor = vec4(base * darken, 1.0);
}
