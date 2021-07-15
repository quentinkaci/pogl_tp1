#version 460

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vUv;
layout(location = 2) in vec3 vNormal;

uniform mat4 world_to_cam_matrix;
uniform mat4 depth_world_to_cam_matrix;

out vec2 uv;
out vec4 shadow_coord;

void main() {
    gl_Position = world_to_cam_matrix * vec4(vPosition, 1.f);
    shadow_coord = depth_world_to_cam_matrix * vec4(vPosition, 1.f);
    uv = vUv;
}