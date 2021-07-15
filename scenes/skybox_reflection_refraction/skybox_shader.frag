#version 460

uniform samplerCube texture_sampler;

in vec3 texture_coordinates;

out vec4 output_color;

void main() {
    output_color = texture(texture_sampler, texture_coordinates);
}