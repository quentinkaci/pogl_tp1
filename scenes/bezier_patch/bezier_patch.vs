#version 460

layout(location = 1) in vec3 vPosition;
layout(location = 2) in vec3 vNormal;

uniform mat4 world_to_cam_matrix;
uniform vec3 light_position;

out vec4 light_dir;
out vec4 normal;

void main() {
    gl_Position = world_to_cam_matrix * vec4(vPosition, 1.f);
    normal = vec4(vNormal, 1.f);
    light_dir = normalize(vec4(light_position, 1.f) - vec4(vPosition, 1.f));
}