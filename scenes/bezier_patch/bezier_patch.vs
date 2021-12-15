#version 460

layout(location = 1) in vec3 vPosition;
layout(location = 2) in vec3 vNormal;

uniform mat4 world_to_cam_matrix;
uniform vec3 light_position;

out vec3 light_dir;
out vec3 normal;
out vec3 position;

void main() {
    gl_Position = world_to_cam_matrix * vec4(vPosition, 1.f);
    normal = vNormal;
    light_dir = normalize(light_position - vPosition);
    position = vPosition;
}