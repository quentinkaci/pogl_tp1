#version 460

layout(location = 1) in vec4 vPosition;

uniform mat4 world_to_cam_matrix;
uniform vec4 uniform_color;

out vec4 color;

void main() {
    gl_Position = world_to_cam_matrix * vPosition;
    color = uniform_color;
}