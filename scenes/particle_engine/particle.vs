#version 460

layout(location = 1) in vec3 position;
layout(location = 2) in int life_time;
layout(location = 3) in float size;

uniform mat4 world_to_cam_matrix;

void main() {
    gl_Position = world_to_cam_matrix * vec4(position, 1.f);
    gl_PointSize = size;
}