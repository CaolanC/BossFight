#version 330 core

in vec3 vWorldPos;
in vec3 vCamPos;   // usually better as a uniform, not an interpolated 'in'
in vec3 vNorm;
in vec2 vuV;

uniform sampler2D uTex;

out vec4 FragColor;

void main() {
    // distance-based darkening
    float dist   = length(vCamPos - vWorldPos);
    float darken = clamp(1.0 - dist * 0.05, 0.1, 1.0);

    // sample albedo from texture
    vec3 albedo = texture(uTex, vuV).rgb;

    // simple Blinn-Phong/Phong-ish lighting
    float ambientStrength = 0.2;
    vec3 lightColor = vec3(1.0);

    vec3 N = normalize(vNorm);
    vec3 L = normalize(vCamPos - vWorldPos); // using camera as light dir?
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 V = normalize(vCamPos - vWorldPos);
    vec3 R = reflect(-L, N);
    float spec = pow(max(dot(V, R), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;

    // combine: (ambient + diffuse) * albedo + specular
    vec3 color = (ambientStrength + diffuse) * albedo + specular;

    // apply distance darkening
    color *= darken;

    FragColor = vec4(color, 1.0);
}
