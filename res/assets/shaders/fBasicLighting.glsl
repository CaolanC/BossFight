#version 460 core

out vec4 FragColor;

layout(std140, binding = 0) uniform CameraUBO
{
    mat4 projection_matrix;
    mat4 view_matrix;
    vec4 camera_position;
};

struct DirectionalLight {
    vec4 position;
    vec4 color;
};

struct AmbientLighting {
    vec4 color; // xyz = color, w = intensity
};

layout(std140, binding = 1) uniform LightingUBO
{
    AmbientLighting ambient_lighting;
};

//vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
//
//};

in vec3 vWorldPos;

void main() {
    float dist = length(camera_position.xyz - vWorldPos);
    float darken = clamp(1.0 - dist * 0.05, 0.1, 1.0);

    vec3 ambient = ambient_lighting.color.rgb * ambient_lighting.color.w;
    FragColor = vec4(ambient, 1.0) * darken;
}
