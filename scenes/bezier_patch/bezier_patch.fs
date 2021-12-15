#version 460

uniform vec3 color;
uniform samplerCube texture_sampler;
uniform vec3 camera_position;
uniform bool phong;

in vec3 normal;
in vec3 light_dir;
in vec3 position;

out vec4 out_color;

void main() {
    if (phong)
        out_color = vec4(dot(normal, light_dir) * color, 1.f);
    else
    {
        vec3 view_vect = normalize(position - camera_position);
        vec3 reflection = reflect(view_vect, normalize(normal));

        out_color = vec4(texture(texture_sampler, reflection).rgb, 1.0);
    }
}