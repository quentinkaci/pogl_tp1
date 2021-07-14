#version 460

uniform samplerCube texture_sampler;
uniform vec3 camera_position;

in vec3 position;
in vec3 normal;

out vec4 output_color;

void main() {
    vec3 view_vect = normalize(position - camera_position);
    vec3 reflection = reflect(view_vect, normalize(normal));

    output_color = vec4(texture(texture_sampler, reflection).rgb, 1.0);
}