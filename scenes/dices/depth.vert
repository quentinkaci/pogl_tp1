#version 460

layout(location = 0) in vec3 vPosition;

uniform mat4 world_to_cam_matrix;

void main() {
	gl_Position =  world_to_cam_matrix * vec4(vPosition, 1.f);
}