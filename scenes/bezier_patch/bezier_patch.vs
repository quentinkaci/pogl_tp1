#version 460

layout(location = 1) in vec3 position;

uniform mat4 world_to_cam_matrix;

void main() {
    gl_Position = world_to_cam_matrix * vec4(position, 1.f);
}