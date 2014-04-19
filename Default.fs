#version 330 core

in vec2 uv;
in vec3 vertex_to_camera;
in vec3 vertex_to_light;
in vec3 normal;

uniform vec3 object_color;
uniform sampler2D sampler;

out vec3 color;

void main()
{
	color = texture(sampler, uv).rgb;// * clamp(dot(normalize(normal), normalize(vertex_to_light)), 1, 1);
}
