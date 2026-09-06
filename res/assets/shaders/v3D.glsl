#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 uV;

layout (std140, binding=0) uniform CameraUBO {
    mat4 projection_matrix;
    mat4 view_matrix;
    vec4 camera_position;
};

uniform mat4 uModel;

out vec3 vWorldPos;
out vec3 vNorm;
out vec2 vuV;

void main() {
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;

    vNorm = mat3(uModel) * aNorm;
    vuV = uV;

    gl_Position = projection_matrix * view_matrix * worldPos;
}
