#pragma once

struct alignas(16) GPU_SDF {
	glm::mat4 transform;
	int type;
	float radius;
};
