#version 460

uniform sampler2D texture_sampler;

in vec2 uv;
in vec4 normal;
in vec4 light_dir;

out vec3 output_color;

void main() {
    output_color = clamp((dot(normal, light_dir)) * texture(texture_sampler, uv).rgb, 0.f, 1.f);
    // output_color = texture(texture_sampler, uv).rgb;
}