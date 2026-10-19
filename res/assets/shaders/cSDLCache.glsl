#version 460

#define TEXTURE_SIZE 64
// Don't know if this is the correct value, be sure to check you spoon.

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

layout(rgba32f, binding = 0) uniform image3D sd_field;

layout(std140, binding = 0) uniform CameraUBO {
	mat4 projection_matrix;
	mat4 view_matrix;
	vec4 camera_position;
};

float smin(float a, float b, float k) {
    float h = max(k - abs(a - b), 0.0) / k;
    return min(a, b) - h * h * k * 0.25;
}


float sd_sphere(uvec3 p, vec3 sphere_position, float radius) {
	return distance(p, sphere_position) - radius;
}

vec3 GetSphereNormal(uvec3 p, vec3 sphere_location, float radius) {
    //vec3 localP = (inverse(uModel) * vec4(p, 1.0)).xyz;
    uvec2 e = uvec2(0.005, 0.0);
    vec3 normal = vec3(
		sd_sphere(p+ e.xyy, sphere_location, radius),
		sd_sphere(p+ e.yxy, sphere_location, radius),
		sd_sphere(p+ e.yyx, sphere_location, radius)
    );
    return normalize(normal);
}

void main() {
	vec4 value = vec4(0.0, 0.0, 0.0, 1.0);
	vec3 sphere_location = vec3(32.0);
	value.w = sd_sphere(gl_GlobalInvocationID, sphere_location, 25.0);
	value.xyz = GetSphereNormal(gl_GlobalInvocationID.xyz, sphere_location, 25.0);
	imageStore(sd_field, ivec3(gl_GlobalInvocationID.xyz), value);
}
