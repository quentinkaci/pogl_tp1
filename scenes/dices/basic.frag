#version 460

uniform sampler2D texture_sampler;

in vec2 uv;
in vec4 normal;
in vec4 light_dir;

out vec4 output_color;

void main() {
    float ambient_light = 0.2;
    float light_intensity = dot(normal, light_dir);
    vec3 text_color = texture(texture_sampler, uv).rgb;

    output_color = vec4(clamp(light_intensity, ambient_light, 1.f) * text_color, 1.f);
}