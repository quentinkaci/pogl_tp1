#version 460

uniform samplerCube texture_sampler;
uniform vec3 camera_position;

in vec3 position;
in vec3 normal;

out vec4 output_color;

void main() {
    vec3 I = normalize(position - camera_position);
    vec3 R = reflect(I, normalize(normal));

    output_color = vec4(texture(texture_sampler, R).rgb, 1.0);
}