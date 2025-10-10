#version 330 core

in vec3 vWorldPos;
in vec3 vCamPos;
in vec3 vNorm;

out vec4 FragColor;

void main() { // expect 4 (FLOAT)
    float dist = length(vCamPos - vWorldPos);
    float darken = clamp(1.0 - dist * 0.05, 0.1, 1.0);
    vec3 base = vec3(0.0, 1.0, 0.1);
    float ambient_strength = 0.2;
    vec3 light_color = vec3(1.0, 1.0, 1.0);

    vec3 norm = normalize(vNorm);
    vec3 lightDir = normalize(vCamPos - vWorldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light_color;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(vCamPos - vWorldPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * light_color;

    vec3 result = (ambient_strength + diffuse + specular) * base;
    FragColor =  vec4(result, 1.0);
}
