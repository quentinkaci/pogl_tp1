#version 460

uniform vec3 color;

in vec4 normal;
in vec4 light_dir;

out vec4 out_color;

void main() {
    out_color = vec4(dot(normal, light_dir) * color, 1.f);
}