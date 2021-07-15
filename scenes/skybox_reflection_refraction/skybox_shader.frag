#version 460

uniform samplerCube texture_sampler;

in vec3 position;

out vec4 output_color;

void main() {
    output_color = texture(texture_sampler, position);
}