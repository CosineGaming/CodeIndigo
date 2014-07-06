#version 120
#pragma unroll
#pragma optionNV unroll all

attribute vec3 V_Position;
attribute vec2 V_UV;

attribute vec3 V_Normal;
attribute vec3 V_Bump_X;
attribute vec3 V_Bump_Y;

uniform mat4 V_MVP;
uniform mat4 V_Model;
uniform mat4 V_View;

uniform vec4 V_Lights[8];
uniform vec4 V_Lamp_Directions;

// Automatic: varying vec3 gl_Position
varying vec2 F_UV;

varying vec3 F_Normal;
varying vec3 F_To_Camera;
varying vec4 F_To_Lights[8];
varying vec4 F_Lamp_Directions[8];

void Render_Requirements()
{
	gl_Position = V_MVP * vec4(V_Position, 1);
	F_UV = V_UV;
}

void Lighting_Requirements()
{
	//mat3 bump_space = transpose(mat3(normalize(V_View*V_Model*vec4(V_Bump_X, 0)).xyz, normalize(V_View*V_Model*vec4(V_Bump_Y, 0)).xyz, normalize(V_View*V_Model*vec4(V_Normal, 0)).xyz));
	//vec3 world_normal = (V_View*V_Model*vec4(V_Normal, 0)).xyz;
	//vec3 world_f_to_camera = -1 * ((V_View*V_Model*vec4(V_Position, 1)).xyz);
	//F_Normal = bump_space*world_normal;
	//F_To_Camera = bump_space*world_f_to_camera;
	//for (int i = 0; i<V_Lights.length(); ++i)
	//{
	//	if (V_Lights[i].w > 0.5)
	//	{
	//		vec3 world_f_to_light = (world_f_to_camera + (V_View*V_Lights[i]).xyz);
	//		F_To_Lights[i] = vec4(bump_space*world_f_to_light, 1);
	//	}
	//	else
	//	{
	//		vec3 world_f_to_light = -1 * (V_View*V_Lights[i]).xyz;
	//		F_To_Lights[i] = vec4(bump_space*world_f_to_light, 0);
	//	}
	//}
	mat3 bump_space = transpose(mat3(normalize(V_Model * vec4(V_Bump_X, 0)).xyz, normalize(V_Model * vec4(V_Bump_Y, 0)).xyz, normalize(V_Model * vec4(V_Normal, 0)).xyz));
	F_Normal = bump_space*(V_Model*vec4(V_Normal, 0)).xyz;
	F_To_Camera = bump_space*(V_Model*vec4(V_Position, 1)).xyz;
	for (int i = 0; i<V_Lights.length(); ++i)
	{
		if (V_Lights[i].w > 0.5)
		{
			F_To_Lights[i] = vec4(bump_space*(V_Lights[i].xyz) + F_To_Camera, 1);
		}
		else
		{
			F_To_Lights[i] = vec4(-1 * bump_space*V_Lights[i].xyz, 0);
		}
		if (V_Lamp_Directions.w < 0.5)
		{
			F_Lamp_Directions[i] = vec4(bump_space * V_Lamp_Directions.xyz, 0);
		}
		else
		{
			F_Lamp_Directions[i] = vec4(0, 0, 0, 1);
		}
	}
}
