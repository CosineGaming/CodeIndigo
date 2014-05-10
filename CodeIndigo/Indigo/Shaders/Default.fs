#version 120

varying vec3 F_Position;
varying vec2 F_UV;
varying vec3 F_Normal;
varying vec3 F_To_Camera;
varying vec4 F_To_Light;

uniform vec4 V_Light;
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
	vec3 color = F_Color.xyz * texture2D(F_Sampler, F_UV).rgb;
	if (F_Light_Enabled == 1)
	{
		if (F_Light_Power == 0)
		{
			gl_FragColor = vec4(vec3(F_Ambient, F_Ambient, F_Ambient) * color, F_Color.a);
		}
		else
		{
			vec3 final_normal = normalize(F_Normal);
			if (!gl_FrontFacing)
			{
				final_normal *= -1;
			}
			float cosine_theta = clamp(dot(final_normal, normalize(F_To_Light.xyz)), 0,1);
			float cosine_alpha = clamp(dot(normalize(F_To_Camera), reflect(-1 * F_To_Light.xyz, final_normal)), 0,1);
			float light_distance_squared = length(V_Light.xyz - F_Position)*length(V_Light.xyz - F_Position);//dot(F_To_Light.xyz, F_To_Light.xyz);
			gl_FragColor = vec4(
				// Ambient
				vec3(F_Ambient, F_Ambient, F_Ambient) * color
				// Diffuse
				+ (F_To_Light.w == 1 ? (F_Light_Color * color * cosine_theta * F_Light_Power / light_distance_squared) : (F_Light_Color * color * cosine_theta))
				// Specular
				//+ (F_To_Light.w == 1 ? F_Light_Color * pow(cosine_alpha, F_Shininess) * F_Light_Power / light_distance_squared : F_Light_Color * pow(cosine_alpha, F_Shininess))
				// Alpha
				, F_Color.a);
		}
	}
	else
	{
		gl_FragColor = vec4(color, F_Color.a);
	}
}
