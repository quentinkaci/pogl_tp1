#version 460

uniform sampler2D texture_sampler;
uniform sampler2DShadow shadow_texture;

in vec2 uv;
in vec4 shadow_coord;

out vec4 output_color;

void main() {
	vec3 light_color = vec3(1, 1, 1);
	vec3 material_color = texture(texture_sampler, uv).rgb;

	float light_intensity = 1.0;
	if (texture(shadow_texture, vec3(shadow_coord.xy, 1)) < shadow_coord.z - 0.005)
		light_intensity = 0.1;

	output_color = vec4(light_intensity * material_color * light_color, 1);
}
