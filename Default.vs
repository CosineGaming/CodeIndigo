#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform vec3 in_light;

out vec2 uv;
out vec3 vertex_to_camera;
out vec3 vertex_to_light;
out vec3 normal;

void main()
{
	gl_Position = MVP * vec4(position, 1);
	uv = in_uv;
	vertex_to_camera = vec3(0, 0, 0) - (V*M*vec4(position, 1)).xyz;
	vertex_to_light = vertex_to_camera + (V*vec4(in_light, 1)).xyz;
	normal = (V*M*vec4(in_normal, 0)).xyz;
}
