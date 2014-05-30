#version 120

varying vec2 F_UV;
varying vec3 F_Normal;
varying vec3 F_To_Camera;
varying vec4 F_To_Light;

uniform vec4 F_Color;
uniform sampler2D F_Sampler;
uniform float F_Ambient;
uniform float F_Shininess;
uniform vec3 F_Light_Color;
uniform int F_Light_Enabled;
varying vec4 F_Light_Direction;

// Automatic: out vec4 gl_FragColor;

vec4 Get_Standard_Color()
{
	return F_Color * texture2D(F_Sampler, F_UV);
}

vec4 Get_Standard_Lighting()
{
	vec4 color = Get_Standard_Color();
	if (F_Light_Enabled == 1 && F_Light_Color != vec3(0,0,0))
	{
		float cosine_theta = clamp(dot(normalize(F_Normal), normalize(F_To_Light.xyz)), 0, 1);
		float cosine_alpha = clamp(dot(normalize(F_To_Camera), reflect(normalize(-1 * F_To_Light.xyz), normalize(F_Normal))), 0, 1);
		float camera_distance_squared = dot(F_To_Camera, F_To_Camera);
		float distance_aspect = 1;
		float direction_impact = 1;
		if (F_To_Light.w > 0.5)
		{
			float light_distance_squared = dot(F_To_Light.xyz, F_To_Light.xyz);
			distance_aspect = 1 / light_distance_squared;
		}
		if (F_Light_Direction.w > 0.5)
		{
			direction_impact = pow(clamp(dot(normalize(F_To_Light), normalize(F_Light_Direction)), 0,1), 1);
		}
		return vec4(
			// Ambient
			vec3(F_Ambient, F_Ambient, F_Ambient) * color.rgb
			// Diffuse
			+ F_Light_Color * color.rgb * cosine_theta * distance_aspect * direction_impact
			// Specular
			//+ 0.25 * F_Light_Color * pow(cosine_alpha, F_Shininess) * distance_aspect * direction_impact
			// Fog
			+ (camera_distance_squared / 1000000l) * vec3(1, 1, 1)
			// Alpha
			, color.a);
	}
	else
	{
		return color;
	}
}
