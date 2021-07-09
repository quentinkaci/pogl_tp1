#version 460

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vUv;

uniform mat4 world_to_cam_matrix;
uniform vec4 light_position;

out vec2 uv;

void main() {
    gl_Position = world_to_cam_matrix * vec4(vPosition, 1.f);
    uv = vUv;
}