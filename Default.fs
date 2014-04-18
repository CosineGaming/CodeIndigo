#version 330 core

in vec2 uv;

uniform vec3 object_color;
uniform sampler2D sampler;

out vec3 color;

void main()
{
	color = texture(sampler, uv).rgb;
}
