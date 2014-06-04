#version 120

varying vec2 F_UV;
varying vec3 F_Normal;

varying vec3 F_To_Camera;
varying vec4 F_To_Lights[8];

uniform vec4 F_Color;
uniform sampler2D F_Sampler;
uniform vec3 F_Ambient;
uniform float F_Shininess;

uniform int F_Lighting_Enabled;
uniform vec3 F_Light_Colors[8];
uniform vec4 F_N_Lamp_Directions[8];
uniform float F_Lamp_Angles[8];
uniform int F_Number_Of_Lights;

// Automatic: out vec4 gl_FragColor;

vec4 Get_Standard_Color()
{
	return F_Color * texture2D(F_Sampler, F_UV);
}

vec4 Get_Standard_Lighting()
{
	vec4 color = Get_Standard_Color();
	if (F_Lighting_Enabled == 1)
	{
		vec4 o_color = vec4(0, 0, 0, color.a);
		o_color.rgb += F_Ambient * color.rgb;
		vec3 n_normal = normalize(F_Normal);
		vec3 n_f_to_camera = normalize(F_To_Camera);
		float camera_distance_squared = dot(F_To_Camera, F_To_Camera);
		for (int i = 0; i < F_Number_Of_Lights; ++i)
		{
			if (F_Light_Colors[i] != vec3(0, 0, 0))
			{
				vec3 n_f_to_light = normalize(F_To_Lights[i].xyz);
				float cosine_theta = clamp(dot(n_normal, n_f_to_light), 0, 1);
				float cosine_alpha = clamp(dot(n_f_to_camera, reflect(normalize(-1 * n_f_to_light), n_normal)), 0, 1);
				float impacts = 1;
				if (F_To_Lights[i].w > 0.5)
				{
					impacts = 1 / dot(F_To_Lights[i].xyz, F_To_Lights[i].xyz);
				}
				if (F_N_Lamp_Directions[i].w > 0.5)
				{
					impacts *= pow(clamp(dot(n_f_to_light, -1 * F_N_Lamp_Directions[i].xyz), 0, 1), 1);
				}
				o_color.rgb += F_Light_Colors[i] * color.rgb * cosine_theta * impacts
					//+ 0.25 * F_Light_Colors[i] * pow(cosine_alpha, F_Shininess) * impacts
					;
			}
		}
		return o_color;
	}
	else
	{
		return color;
	}
}
