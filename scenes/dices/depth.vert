#version 460

layout(location = 0) in vec3 vPosition;

uniform mat4 depth_MVP;

void main() {
	gl_Position =  depth_MVP * vec4(vPosition, 1.f);
}