#version 460

#define MAX_STEPS 100
#define MAX_DIST 200.0
#define HIT_THRESHOLD 0.01

in vec3 FragPos;
out vec4 FragColor;

layout(std140, binding = 0) uniform CameraUBO
{
	mat4 projection_matrix;
	mat4 view_matrix;
	vec4 camera_position;
};

float sd_sphere(vec3 pos, float radius) {
	pos.x = pos.x - round(pos.x);
	float distance = length(pos) - radius;
	//distance += cos(pos.x) * 0.5;
	return distance;
}

vec3 GetNormal(vec3 p) {
	vec2 e = vec2(.01, 0);
	float d = sd_sphere(p, 0.5);
	vec3 normal = d - vec3(
		sd_sphere(p-e.xyy, 0.5),
		sd_sphere(p-e.yxy, 0.5),
		sd_sphere(p-e.yyx, 0.5)

	);
	return normalize(normal);
}

float GetLight(vec3 p) {
	vec3 light = vec3(4.0, 4.0, 2.0);
	vec3 light_vector = normalize(light - p);
	vec3 surface_normal = GetNormal(p);
	return clamp(dot(light_vector, surface_normal), 0., 1.);
}

float RayMarcher(vec3 ray_origin, vec3 ray_direction) {
	float ray_length = 0.0;

	for(int i = 0; i < MAX_STEPS; i++) {
		vec3 p = ray_origin + ray_direction * ray_length;
		float dist_scene = sd_sphere(p, 0.5);
		ray_length += dist_scene;
		if (ray_length >= MAX_DIST || dist_scene <= HIT_THRESHOLD) break;
	}
	return ray_length;
}

void main() {
	vec3 ray_dir = normalize(FragPos - camera_position.xyz);
	float t = RayMarcher(camera_position.xyz, ray_dir);

	vec3 position = camera_position.xyz + ray_dir * t;

	float dif = GetLight(position);

	vec3 col = vec3(dif); //+ GetNormal(position);
	col = GetNormal(position);
	//col = vec3(dif);
	//col += GetNormal(position);

	if (t < MAX_DIST) {
		FragColor = vec4(col, 1.0);
	} else {
		FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}
