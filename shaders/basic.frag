#version 460

uniform sampler2D texture_sampler;

in vec2 uv;

out vec3 output_color;

void main() {
    output_color = texture(texture_sampler, uv).rgb;
}