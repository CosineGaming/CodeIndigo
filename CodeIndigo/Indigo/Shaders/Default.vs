#version 120

attribute vec3 V_Position;
attribute vec2 V_UV;
attribute vec3 V_Normal;

uniform mat4 M_All;
uniform mat4 M_Model;
uniform mat4 M_View;

uniform vec4 V_Light;

// Automatic: varying vec3 gl_Position
varying vec2 F_UV;
varying vec3 F_Normal;
varying vec3 F_To_Camera;
varying vec4 F_To_Light;
varying vec3 F_Distance_To_Light;

void main()
{
	gl_Position = M_All * vec4(V_Position, 1);
	F_UV = V_UV;
	F_Normal = (M_View*M_Model*vec4(V_Normal, 0)).xyz;
	F_To_Camera = -1 * (M_View*M_Model*vec4(V_Position, 1)).xyz;
	if (V_Light.w == 1)
	{
		F_To_Light = vec4(F_To_Camera + (M_View*V_Light).xyz, 1);
	}
	else
	{
		F_To_Light = -1 * (M_View * V_Light);
	}
}
