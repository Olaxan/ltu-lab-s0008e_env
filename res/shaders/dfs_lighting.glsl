#version 430

#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1
#define LIGHT_SPOT 2

struct light_base
{
	vec3 color;
	float ambient_intensity;
	float diffuse_intensity;
};

struct attenuation
{
	float constant;
	float linear;
	float exponential;
};

struct light_source
{
	light_base base;
	vec3 position;
	vec3 direction;
	attenuation falloff;
	int type;
};

out vec4 Color;

in vec2 Uv;

layout (location = 0) uniform sampler2D g_position;
layout (location = 1) uniform sampler2D g_normal;
layout (location = 2) uniform sampler2D g_albedo;
layout (location = 3) uniform sampler2D g_orm;
layout (location = 4) uniform sampler2D g_depth;

uniform vec4 cam_pos;
uniform mat4 light_model;
uniform int light_type;

uniform light_source source;

void main()
{

    vec3 ambient = source.ambient_intensity * source.base.color;

	vec3 normal = texture(g_normal, Uv).rgb;
	vec3 fragment = texture(g_position, Uv).rgb;

	vec3 source_dir = normalize(source.position - fragment);
	vec3 view_dir = normalize(cam_pos.xyz - fragment);
	vec3 halfway_dir = normalize(source_dir + view_dir);
	
	float diff = max(dot(normal, source_dir), 0.0);
	vec3 diffuse = diff * source.base.color;

	float specular_power = max(1.0, 2 / pow(texture(g_orm, Uv).g, 4.0) - 2);	
	float spec = pow(max(dot(normal, halfway_dir), 0.0), specular_power);
	vec3 specular = specular_strength * spec * source.color;
	
	vec3 result = (ambient + diffuse + specular);
	
	Color = vec4(result * texture(g_albedo, Uv).rgb, 1.0); // * texture(g_orm, Uv).r;
	gl_FragDepth = texelFetch(g_depth, ivec2(gl_FragCoord.xy), 0).r;
}
