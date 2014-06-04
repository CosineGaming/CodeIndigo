#version 120

attribute vec3 V_Position;
attribute vec2 V_UV;

attribute vec3 V_Normal;

uniform mat4 V_MVP;
uniform mat4 V_Model;
uniform mat4 V_View;

uniform vec4 V_Lights[8];

// Automatic: varying vec3 gl_Position
varying vec2 F_UV;

varying vec3 F_Normal;
varying vec3 F_To_Camera;
varying vec4 F_To_Lights[8];

void Render_Requirements()
{
	gl_Position = V_MVP * vec4(V_Position, 1);
	F_UV = V_UV;
}

void Lighting_Requirements()
{
	F_Normal = (V_View*V_Model*vec4(V_Normal, 0)).xyz;
	F_To_Camera = -1 * (V_View*V_Model*vec4(V_Position, 1)).xyz;
	for (int i = 0; i < 8; ++i)
	{
		if (V_Lights[i].w == 1)
		{
			F_To_Lights[i] = vec4(F_To_Camera + (V_View*V_Lights[i]).xyz, 1);
		}
		else
		{
			F_To_Lights[i] = vec4(-1 * (V_View * V_Lights[i]).xyz, 0);
		}
	}
}