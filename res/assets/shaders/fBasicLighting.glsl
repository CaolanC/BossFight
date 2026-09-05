#version 330 core

out vec4 FragColor;

layout(std140, binding = 0) uniform CameraUBO
{
    mat4 view_matrix;
    mat4 projection_matrix;
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

void main() {
    vec3 ambient = ambient_lighting.color.rgb * ambient_lighting.color.w;
    FragColor = vec4(ambient, 1.0);
}
