#version 120

varying vec2 F_UV;
varying vec3 F_Normal;
varying vec3 F_To_Camera;
varying vec4 F_To_Light;

uniform vec4 F_Color;
uniform sampler2D F_Sampler;
uniform float F_Light_Power;
uniform float F_Ambient;
uniform float F_Shininess;
uniform vec3 F_Light_Color;
uniform int F_Light_Enabled;

// Automatic: out vec4 gl_FragColor;

void main()
{
	vec3 color = F_Color.rgb * texture2D(F_Sampler, F_UV).rgb;
	if (F_Light_Enabled == 1)
	{
		if (F_Light_Power == 0)
		{
			gl_FragColor = vec4(F_Ambient * color, F_Color.a);
		}
		else
		{
			float cosine_theta = clamp(dot(normalize(F_Normal), normalize(F_To_Light.xyz)), 0, 1);
			float cosine_alpha = clamp(dot(normalize(F_To_Camera), reflect(normalize(-1 * F_To_Light.xyz), normalize(F_Normal))), 0, 1);
			float camera_distance_squared = dot(F_To_Camera.xyz, F_To_Camera.xyz);
			float distance_aspect = 1;
			if (F_To_Light.w > 0.5)
			{
				float light_distance_squared = dot(F_To_Light.xyz, F_To_Light.xyz);
				distance_aspect = F_Light_Power / light_distance_squared;
			}
			gl_FragColor = vec4(
				// Ambient
				vec3(F_Ambient, F_Ambient, F_Ambient) * color
				// Diffuse
				+ F_Light_Color * color * cosine_theta * distance_aspect
				// Specular
				+ 0.25 * F_Light_Color * pow(cosine_alpha, F_Shininess) * distance_aspect
				// Fog
				+ (camera_distance_squared / 1000000) * vec3(1, 1, 1)
				// Alpha
				, F_Color.a);
		}
	}
	else
	{
		gl_FragColor = vec4(color, F_Color.a);
	}
}
