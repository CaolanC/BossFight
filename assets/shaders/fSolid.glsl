#version 330 core

in vec3 vWorldPos;
in vec3 vCamPos;
in vec3 vNorm;

out vec4 FragColor;

void main() { // expect 4 (FLOAT)
    float dist = length(vCamPos - vWorldPos);
    float darken = clamp(1.0 - dist * 0.05, 0.1, 1.0);
    vec3 base = vec3(0.9, 0.5, 0.7);
    float ambient_strength = 0.2;
    vec3 light_color = vec3(1.0, 1.0, 1.0);

    vec3 norm = normalize(vNorm);
    vec3 lightDir = normalize(vCamPos - vWorldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light_color;
    vec3 result = (ambient_strength + diffuse) * base;
    FragColor =  vec4(result, 1.0);
}
