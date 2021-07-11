#version 460

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vUv;
layout(location = 2) in vec3 vNormal;

uniform mat4 world_to_cam_matrix;
uniform vec4 light_position;

out vec2 uv;
out vec4 normal;
out vec4 light_dir;

void main() {
    gl_Position = world_to_cam_matrix * vec4(vPosition, 1.f);
    uv = vUv;
    normal = vec4(vNormal, 1.f);
    light_dir = normalize(light_position - vec4(vPosition, 1.f));
}