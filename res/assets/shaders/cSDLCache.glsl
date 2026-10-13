#version 460

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

layout(rgba32f, binding = 0) uniform image3D sd_field;

float sd_sphere(uvec3 p, vec3 sphere_position, float radius) {
	return distance(p, -sphere_position) - radius;
}

void main() {
	vec4 value = vec4(0.0, 0.0, 0.0, 1.0);
	value.x = sd_sphere(gl_GlobalInvocationID, vec3(4.0, 4.0, 4.0), 0.5);
	imageStore(sd_field, ivec3(gl_GlobalInvocationID.xyz), value);
}
