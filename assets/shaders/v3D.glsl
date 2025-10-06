#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;
uniform vec3 uCamPos;

out vec3 vWorldPos;
out vec3 vCamPos;

void main() {
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;   // ensures uModel is used
    vCamPos   = uCamPos;        // ensures uCamPos is used

    gl_Position = uProjection * uView * worldPos; // ensures uProjection/uView are used
}
