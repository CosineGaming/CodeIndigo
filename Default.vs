#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 in_uv;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;

out vec2 uv;

void main()
{
	vec4 model = vec4(position, 1);
	gl_Position = MVP * model;
	uv = in_uv;
}
