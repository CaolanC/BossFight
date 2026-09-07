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
    vec4 color; 
};

#define MAX_LIGHTS 100

layout(std140, binding = 1) uniform LightingUBO
{
    AmbientLighting ambient_lighting;
    PointLight[MAX_LIGHTS] point_lights;
    int no_lights;

};

//vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
//
//};

uniform sampler2D uTex;

in vec3 FragPos;
in vec3 vNorm;
in vec2 vUv;

vec3 calc_diffuse(vec3 light_dir, vec3 norm) {
    float diff = max(dot(norm, light_dir), 0.0);

    return diff * vec3(1.0f, 1.0f, 1.0f); // Custom light colors later
};

vec3 calc_specular(vec3 light_dir, vec3 norm) {
    float specular_strength = 0.5; // We can probabaly add this to the lighting ubo later
    vec3 view_dir = normalize(camera_position.xyz - FragPos);
    vec3 reflect_dir = reflect(-light_dir, norm);

    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    
    return specular_strength * spec * vec3(1.0, 1.0, 1.0); // Custom light colors later
};

void main() {

    vec3 ambient = ambient_lighting.color.rgb * ambient_lighting.color.w;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    vec3 norm = normalize(vNorm);
    vec3 light_dir;
    for(int i = 0; i < no_lights; i++) {
	PointLight pl = point_lights[i];
        light_dir = normalize(pl.position.xyz - FragPos);
    	diffuse += calc_diffuse(light_dir, norm);
    	specular += calc_specular(light_dir, norm);
    };

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
