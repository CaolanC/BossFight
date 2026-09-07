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

struct PointLight {
    vec4 position; // xyz = position, w = intensity. Dunno whether to keep that for the w component
};

#define NO_PT_LIGHTS 1

layout(std140, binding = 1) uniform LightingUBO
{
    AmbientLighting ambient_lighting;
    PointLight[NO_PT_LIGHTS] point_lights;
};

//vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
//
//};

uniform sampler2D uTex;

in vec3 vWorldPos;
in vec3 vNorm;
in vec2 vUv;

void main() {

    vec3 point_light = vec3(0);

    vec3 norm = normalize(vNorm);
    vec3 light_dir = normalize(vNorm - vWorldPos);

    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * vec3(1.0f, 1.0f, 1.0f); // Custom light colors later

    vec3 ambient = ambient_lighting.color.rgb * ambient_lighting.color.w;
    FragColor = vec4(ambient + diffuse, 1.0);
}
