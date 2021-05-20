#version 460

in vec4 normal;
in vec4 color;
in vec4 light_dir;

out vec4 output_color;

void main() {
    output_color = clamp((dot(normal, light_dir)) * color, 0.f, 1.f);
}