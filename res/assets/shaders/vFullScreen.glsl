#version 460

precision highp float;

uniform mat4 uModel;

out vec3 FragPos;

layout (std140, binding=0) uniform CameraUBO {
    mat4 projection_matrix;
    mat4 view_matrix;
    vec4 camera_position;
};

void main() {
    // Generate full-screen triangle coordinates from gl_VertexID (0, 1, or 2)
    float x = float((gl_VertexID & 1) << 2) - 1.0;
    float y = float((gl_VertexID & 2) << 1) - 1.0;
    vec3 p = vec3(x, y, 0.0);
    vec4 worldPos = uModel * vec4(p, 1.0);
    FragPos = worldPos.xyz;
    gl_Position = projection_matrix * view_matrix * worldPos;
}
