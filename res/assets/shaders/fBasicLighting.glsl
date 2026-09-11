#version 460

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
    vec4 color;
};

struct PointLight {
    vec4 position;
    vec4 color; 
};

#define MAX_LIGHTS 100

layout(std140, binding = 1) uniform LightingUBO
{
    AmbientLighting ambient_lighting;
    PointLight[MAX_LIGHTS] point_lights;
    int no_lights;

};

uniform sampler2D uTex;

in vec3 FragPos;
in vec3 vNorm;
in vec2 vUV;

vec3 calc_diffuse(vec3 light_dir, vec3 norm, vec4 light_col) {
    float diff = max(dot(norm, light_dir), 0.0);

    return diff * light_col.xyz;
}

vec3 calc_specular(vec3 light_dir, vec3 norm, vec4 light_col) {
    float specular_strength = 0.5; // We can probabaly add this to the lighting ubo later
    vec3 view_dir = normalize(camera_position.xyz - FragPos);
    vec3 reflect_dir = reflect(-light_dir, norm);

    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32.0);
    
    return specular_strength * spec * light_col.xyz;
}

void main() {

    //vec2 uv = vec2(vUV.x, 1.0 - vUV.y);
    vec3 ambient = ambient_lighting.color.rgb * ambient_lighting.color.w;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    vec3 norm = normalize(vNorm);
    vec3 light_dir;
    for(int i = 0; i < no_lights; i++) {
        PointLight pl = point_lights[i];
        light_dir = normalize(pl.position.xyz - FragPos);
	    float distance = length(pl.position.xyz - FragPos);
        float attenuation = 1.0 / (1.0 + 0.5 * distance + 0.3 * (distance * distance));
    	diffuse += calc_diffuse(light_dir, norm, pl.color) * attenuation;
    	specular += calc_specular(light_dir, norm, pl.color) * attenuation;
    }

    vec3 albedo = texture(uTex, vUV).rgb;
	vec3 color = (ambient + diffuse) * albedo + specular;
    FragColor = vec4(color, 1.0);
}
