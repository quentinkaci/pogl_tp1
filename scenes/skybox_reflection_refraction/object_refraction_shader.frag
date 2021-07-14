#version 460

uniform samplerCube texture_sampler;
uniform vec3 camera_position;

in vec3 position;
in vec3 normal;

out vec4 output_color;

void main() {
    vec3 view_vect = normalize(position - camera_position);
    vec3 reflection = texture(texture_sampler, reflect(view_vect, normalize(normal))).rgb;
    vec3 refraction = texture(texture_sampler, refract(view_vect, normalize(normal), 1.0 / 1.33)).rgb;
    vec3 mix_r = mix(reflection, refraction, 0.75);

    output_color = vec4(mix_r, 1.0);
}