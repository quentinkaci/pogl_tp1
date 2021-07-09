#version 460

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec4 vNormal;
layout(location = 2) in vec4 vColor;

uniform mat4 world_to_cam_matrix;
uniform vec4 light_position;

out vec4 normal;
out vec4 color;
out vec4 light_dir;

void main() {
    gl_Position = world_to_cam_matrix * vec4(vPosition, 1.f);
    // normal = vNormal;
    // color = vColor;
    // light_dir = normalize(light_position - vPosition);
}