#version 460

uniform sampler2D texture_sampler;
uniform sampler2DShadow shadowMap;

in vec2 uv;
in vec4 shadow_coord;

out vec4 output_color;

void main() {
	vec3 LightColor = vec3(1, 1, 1);
	
	vec3 MaterialDiffuseColor = texture(texture_sampler, uv).rgb;

	float bias = 0.005;
	float visibility = 1.0;
	if (texture(shadowMap, vec3(shadow_coord.xy, 1)) < shadow_coord.z - bias)
	{
		visibility = 0.1;
	}

	output_color = vec4(visibility * MaterialDiffuseColor * LightColor, 1);
}
