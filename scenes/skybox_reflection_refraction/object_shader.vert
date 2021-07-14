#version 460

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;

uniform mat4 world_to_cam_matrix;
uniform mat4 model;

out vec3 position;
out vec3 normal;

void main() {
    gl_Position = world_to_cam_matrix * vec4(vPosition, 1.f);
    position = vPosition;
    normal = vNormal;
}