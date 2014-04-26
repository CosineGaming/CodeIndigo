#version 330 core

attribute vec3 Position;
attribute vec2 UV;
attribute vec3 Normal;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform vec4 Light;

// Automatic: varying vec3 gl_Position
varying vec2 F_UV;
varying vec3 F_Normal;
varying vec3 F_To_Camera;
varying vec4 F_To_Light;
varying vec3 F_Distance_To_Light;

void main()
{
	gl_Position = MVP * vec4(Position, 1);
	F_UV = UV;
	F_Normal = (V*M*vec4(Normal, 0)).xyz;
	F_To_Camera = -1 * (V*M*vec4(Position, 1)).xyz;
	if (Light.w == 1)
	{
		F_To_Light = vec4(F_To_Camera + (V*Light).xyz, 1);
	}
	else
	{
		F_To_Light = -1 * (V * Light);
	}
}
