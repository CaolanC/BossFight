#version 460

#define MAX_STEPS 40.0
#define MAX_DIST 25.0
#define HIT_THRESHOLD 0.05

in vec3 FragPos;
out vec4 FragColor;

layout(std140, binding = 0) uniform CameraUBO
{
	mat4 projection_matrix;
	mat4 view_matrix;
	vec4 camera_position;
};

float sd_sphere(vec3 pos, float radius) {
	//pos.x = pos.x - round(pos.x);
	float distance = length(pos) - radius;
	//float distance = sin(pos.x);
	//distance += cos(pos.x) * 0.5;
	return distance;
}

vec3 GetNormal(vec3 p) {
	vec2 e = vec2(.01, 0);
	//float d = sd_sphere(p, 0.5);
	vec3 normal = vec3(
		sd_sphere(p-e.xyy, 0.5),
		sd_sphere(p-e.yxy, 0.5),
		sd_sphere(p-e.yyx, 0.5)

	);
	return normalize(normal);
}


float old_sph(ivec3 i, vec3 f, ivec3 c) {
	float rad = 0.1;

	//float rad = 0.5 * (abs(i.y + f.y) / 10);

	return length(f-vec3(c)) - rad;
}

float hash31(vec3 p) {
    p = fract(p * 0.3183099 + vec3(0.1, 0.2, 0.3));
    p *= 17.0;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

float not_so_old_sph(vec3 i, vec3 f, vec3 c) {
    vec3 p = 17.0 * fract(
        (i + c) * 0.3183099 + vec3(0.11, 0.17, 0.13)
    );

    float w = fract(
        p.x * p.y * p.z * (p.x + p.y + p.z)
    );

    float r = 0.7 * w * w;

    return length(f - c) - r;
}

float sph(ivec3 i, vec3 f, ivec3 c) {
    vec3 grid_pos = vec3(i + c);

    float h = hash31(grid_pos);

    float rad = 0.2 + 0.2 * h;

    return length(f - vec3(c)) - rad;
}

float sdBase(vec3 p) {
	ivec3 i = ivec3(floor(p));
	vec3 f = fract(p);

	return min(
		min(
		  min(sph(i, f, ivec3(0, 0, 0)), sph(i, f, ivec3(0, 0, 1))),
		  min(sph(i, f, ivec3(0, 1, 0)), sph(i, f, ivec3(0, 1, 1)))
		),
		min(
		  min(sph(i, f, ivec3(1, 0, 0)), sph(i, f, ivec3(1, 0, 1))),
		  min(sph(i, f, ivec3(1, 1, 0)), sph(i, f, ivec3(1, 1, 1)))
		)
	);
}

float smax(float a, float b, float k) {
    float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return mix(a, b, h) + k * h * (1.0 - h);
}

float old_smin( float a, float b, float k )
{
    k *= 1.0;
    float r = exp2(-a/k) + exp2(-b/k);
    return -k*log2(r);
}

float smin(float a, float b, float k) {
    float h = max(k - abs(a - b), 0.0) / k;
    return min(a, b) - h * h * k * 0.25;
}

float sdFbm( vec3 p, float d )
{
   float s = 5.0;
   for( int i=0; i<2; i++ )
   {
       // evaluate new octave
       float n = s*sdBase(p);
	
       // add
       n = smax(n,d-0.1*s,0.3*s);
       d = smin(n,d      ,0.3*s);
	
       // prepare next octave
       p = mat3( 0.00, 1.60, 1.20,
                -1.60, 0.72,-0.96,
                -1.20,-0.96, 1.28 )*p;
       s = 0.5*s;
   }
   return d;
}

float map(vec3 p) {
    float base_plane = p.y;
    return sdFbm(p, base_plane);
}

vec3 GetNormal2(vec3 p) {
    vec2 e = vec2(0.001, 0.0);
    //float d = map(p);
    vec3 normal = vec3(
	map(p + e.xyy) - map(p - e.xyy),
        map(p + e.yxy) - map(p - e.yxy),
        map(p + e.yyx) - map(p - e.yyx)
    );
    return normalize(normal);
}

float GetLight(vec3 p) {
	vec3 light = vec3(4.0, 4.0, 2.0);
	vec3 light_vector = normalize(light - p);
	vec3 surface_normal = GetNormal2(p);
	return clamp(dot(light_vector, surface_normal), 0., 1.);
}

float RayMarcher(vec3 ray_origin, vec3 ray_direction) {
	float ray_length = 0.0;

	for(int i = 0; i < MAX_STEPS; i++) {
		vec3 p = ray_origin + ray_direction * ray_length;
		//float dist_scene = sd_sphere(p, 0.5);
		float dist_scene = map(p);
		ray_length += dist_scene * 0.4;
		if (ray_length >= MAX_DIST || dist_scene <= HIT_THRESHOLD) break;
	}
	return ray_length;
}

void main() {
	vec3 ray_dir = normalize(FragPos - camera_position.xyz);
	float t = RayMarcher(camera_position.xyz, ray_dir);

	vec3 position = camera_position.xyz + ray_dir * t;

	float dif = GetLight(position);

	//vec3 col = vec3(dif); //+ GetNormal(position);
	vec3 col = GetNormal2(position) * 0.5 + 0.5;
	//col = vec3(dif);
	//col += GetNormal(position);

	if (t < MAX_DIST) {
		FragColor = vec4(col, 1.0);
	} else {
		FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}
