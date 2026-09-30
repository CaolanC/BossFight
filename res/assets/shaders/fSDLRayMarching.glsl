#version 460

in vec3 FragPos;
out vec4 FragColor;

float sd_sphere(vec3 pos, float radius) {
	return length(pos) - radius;
};

void main() {
	float sphere_distance =  sd_sphere(FragPos, 3.0);
	if (sphere_distance <= 0.0) {
		FragColor = vec4(1.0);
	} else {
		FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}
