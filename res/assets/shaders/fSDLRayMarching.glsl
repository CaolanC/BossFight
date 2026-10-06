#version 460

#define MAX_STEPS 50.0
#define MAX_DIST 30.0
#define HIT_THRESHOLD 0.005

#define PLANET_RADIUS 5.0

in vec3 FragPos;
out vec4 FragColor;

uniform mat4 uModel;

layout(std140, binding = 0) uniform CameraUBO
{
	mat4 projection_matrix;
	mat4 view_matrix;
	vec4 camera_position;
};

struct DirectionalLight {
    vec4 position;
    vec4 color;
};

struct AmbientLighting {
    vec4 color;
};

struct PointLight {
    vec4 position;
    vec4 color; 
};

#define MAX_LIGHTS 100

layout(std140, binding = 1) uniform LightingUBO
{
    AmbientLighting ambient_lighting;
    PointLight[MAX_LIGHTS] point_lights;
    int no_lights;

};

float hash31(vec3 p)
{
    p = fract(p * 0.3183099 + vec3(0.1, 0.2, 0.3));
    p *= 17.0;

    return fract(
        p.x * p.y * p.z *
        (p.x + p.y + p.z)
    );
}

float noise(vec3 p)
{
    vec3 i = floor(p);
    vec3 f = fract(p);

    // Smooth interpolation curve.
    f = f * f * (3.0 - 2.0 * f);

    float n000 = hash31(i + vec3(0, 0, 0));
    float n100 = hash31(i + vec3(1, 0, 0));
    float n010 = hash31(i + vec3(0, 1, 0));
    float n110 = hash31(i + vec3(1, 1, 0));

    float n001 = hash31(i + vec3(0, 0, 1));
    float n101 = hash31(i + vec3(1, 0, 1));
    float n011 = hash31(i + vec3(0, 1, 1));
    float n111 = hash31(i + vec3(1, 1, 1));

    float x00 = mix(n000, n100, f.x);
    float x10 = mix(n010, n110, f.x);
    float x01 = mix(n001, n101, f.x);
    float x11 = mix(n011, n111, f.x);

    float y0 = mix(x00, x10, f.y);
    float y1 = mix(x01, x11, f.y);

    return mix(y0, y1, f.z);
}


float fbm(vec3 p)
{
    float value = 0.0;
    float amplitude = 0.5;

    for (int i = 0; i < 6; i++)
    {
        value += noise(p) * amplitude;

        p *= 2.0;
        amplitude *= 0.5;
    }

    return value;
}


float ridgedNoise(vec3 p)
{
    float n = noise(p);

    // Convert valleys of normal noise into ridges.
    n = 1.0 - abs(n * 2.0 - 1.0);

    // Sharpen the ridges.
    n *= n;

    return n;
}


float ridgedFbm(vec3 p)
{
    float value = 0.0;
    float amplitude = 0.5;

    for (int i = 0; i < 5; i++)
    {
        value += ridgedNoise(p) * amplitude;

        p *= 2.0;
        amplitude *= 0.5;
    }

    return value;
}


float terrain(vec3 p)
{
    // IMPORTANT:
    // p is a position in 3D space.
    // Normalize it so the terrain is based only on direction
    // from the planet centre.
    p = normalize(p);

    // Large-scale continental structure.
    float continents = fbm(p * 2.0);

    // Mountain structure.
    float mountains = ridgedFbm(p * 5.0);

    // Small-scale surface detail.
    float detail = fbm(p * 20.0);

    // Remove some of the mountains from low areas.
    float mountainMask = smoothstep(
    	0.35,
    	0.65,
    	continents
    );

    mountains *= mountainMask;

    // Combine scales.
    float height = 0.0;

    height += (continents - 0.5) * 2.0;
    height += mountains * 0.8;
    height += (detail - 0.5) * 0.15;

    return height;
}

float planetSDF(vec3 p)
{
    float r = length(p);

    float height = terrain(p);

    return r - (PLANET_RADIUS + height);
}

float sd_sphere(vec3 pos, float radius) {
	//pos.x = pos.x - round(pos.x);
	float distance = length(pos) - radius;
	//float distance = sin(pos.x);
	//distance += cos(pos.x) * 0.5;
	return distance;
}

float old_sph(ivec3 i, vec3 f, ivec3 c) {
	float rad = 0.1;

	//float rad = 0.5 * (abs(i.y + f.y) / 10);

	return length(f-vec3(c)) - rad;
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
   float s = 10.0;
   for( int i=0; i<3; i++ )
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

vec3 GetNormal3(vec3 p) {
    vec2 e = vec2(0.001, 0.0);
    //float d = map(p);
    vec3 normal = vec3(
	planetSDF(p + e.xyy),
	planetSDF(p + e.yxy),
	planetSDF(p + e.yyx)
    );
    return normalize(normal);
}

vec3 GetLight(vec3 p, PointLight light) {
	//vec3 light = vec3(4.0, 4.0, 2.0);
	//vec3 light = camera_position.xyz;
	vec3 light_vector = normalize(light.position.xyz - p);
	vec3 surface_normal = GetNormal3(p);
	float d = distance(light.position.xyz, p);
	return (vec3((clamp(dot(light_vector, surface_normal), 0., 1.)) / (d/10)) * light.color.xyz) * light.color.w;
}

vec3 lighting(vec3 p) {
	vec3 light_value = vec3(0.0);
	vec3 localP = (inverse(uModel) * vec4(p, 1.0)).xyz;

	for(int i = 0; i < no_lights; i++) {
		PointLight pl = point_lights[i];
		light_value += GetLight(localP, pl);
	}

	//vec4 spotlight = vec4(1.0, 1.0, 1.0, 0.5);
	//light_value += clamp(dot(normalize(p), normalize(camera_position.xyz)), 0., 1.); // * spotlight.xyz * spotlight.w;

	return light_value;
}

float RayMarcher(vec3 ray_origin, vec3 ray_direction) {
	float ray_length = 0.0;

	for(int i = 0; i < MAX_STEPS; i++) {
		vec3 p = ray_origin + ray_direction * ray_length;
		vec3 localP = (inverse(uModel) * vec4(p, 1.0)).xyz;

		float falloff = distance(camera_position.xyz, p);
		//float dist_scene = sd_sphere(p, 0.5);
		float dist_scene = planetSDF(localP);
		//float dist_other_planet = planetSDF(localP);
		vec3 p1 = vec3(7.0, 0.0, 0.0);
		dist_scene = smax(dist_scene, sd_sphere(p-p1, 3.0), 1.0);
		//dist_scene -= dist_other_planet;
		//float dist_scene = map(p);
		ray_length += dist_scene * 0.4;
		if (ray_length >= MAX_DIST || dist_scene <= HIT_THRESHOLD * falloff) break;
	}
	return ray_length;
}

void main() {
	vec3 ray_dir = normalize(FragPos - camera_position.xyz);
	float t = RayMarcher(camera_position.xyz, ray_dir);

	vec3 position = camera_position.xyz + ray_dir * t;

	//float dif = GetLight(position);
	vec3 dif = lighting(position);
	

	vec3 col = vec3(dif); //+ GetNormal(position);
	//vec3 col = vec3(dif) + GetNormal3(position);
	//vec3 col = GetNormal2(position) * 0.5 + 0.5;
	//col = vec3(dif);
	//col += GetNormal(position);

	if (t < MAX_DIST) {
		FragColor = vec4(col, 1.0);
	} else {
		FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}
