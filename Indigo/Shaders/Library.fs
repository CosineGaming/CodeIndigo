#version 120

varying vec2 F_UV;
varying vec3 F_Normal;

varying vec3 F_To_Camera;
varying vec4 F_To_Lights[8];
varying vec4 F_Lamp_Directions[8];

uniform vec4 F_Color;
uniform sampler2D F_Texture;
uniform sampler2D F_Bump_Map;
uniform vec3 F_Ambient;
uniform float F_Shininess;

uniform int F_Lighting_Enabled;
uniform vec3 F_Light_Colors[8];
uniform vec4 F_N_Lamp_Directions[8];
uniform float F_Lamp_Angles[8];
uniform int F_Number_Of_Lights;

// Automatic: out vec4 gl_FragColor;

vec4 Standard_Material_Color()
{
	return F_Color * texture2D(F_Texture, F_UV);
}

vec4 Standard_Lighting(vec4 a_base, bool a_diffuse = true, bool a_specular = true)
{
	vec4 f_color = vec4(0, 0, 0, a_base.a);
	f_color.rgb += F_Ambient * a_base.rgb;
	vec3 n_normal = normalize(texture2D(F_Bump_Map, F_UV).rgb * 2 - 1);
	vec3 n_f_to_camera = normalize(F_To_Camera);
	for (int i = 0; i < F_Number_Of_Lights; ++i)
	{
		if (F_Light_Colors[i] != vec3(0, 0, 0))
		{
			vec3 f_n_to_light = normalize(F_To_Lights[i].xyz);
			float f_cosine_theta = clamp(dot(n_normal, f_n_to_light), 0, 1);
			float f_impacts = 1;
			if (F_To_Lights[i].w > 0.5)
			{
				//f_impacts = 1 / dot(F_To_Lights[i].xyz, F_To_Lights[i].xyz);
			}
			if (F_Lamp_Directions[i].w < 0.5)
			{
				//f_impacts *= pow(clamp(dot(f_n_to_light, -1 * normalize(F_Lamp_Directions[i].xyz)), 0, 1), F_Lamp_Angles[i]);
			}
			if (a_specular)
			{
				if (F_Shininess != 0)
				{
					float f_cosine_alpha = clamp(dot(n_f_to_camera, reflect(-1 * f_n_to_light, n_normal)), 0, 1);
					f_color.rgb += 0.25 * F_Light_Colors[i] * pow(f_cosine_alpha, F_Shininess) * f_impacts;
				}
			}
			if (a_diffuse)
			{
				f_color.rgb += a_base.rgb * F_Light_Colors[i] * f_cosine_theta * f_impacts;
			}
		}
	}
	return f_color;
}

vec4 Light_3D_Only(vec4 a_base, bool a_diffuse = true, bool a_specular = true)
{
	if (F_Lighting_Enabled == 1)
	{
		return Standard_Lighting(a_base, a_diffuse, a_specular);
	}
	else
	{
		return a_base;
	}
}
