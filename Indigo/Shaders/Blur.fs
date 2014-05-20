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
	vec3 start = texture2D(F_Sampler, F_UV).rgb;
	float offset = length(F_To_Camera)*length(F_To_Camera) / 524288l;
	vec2 full_offset = vec2(offset, offset);
	vec2 next = F_UV + full_offset;
	start += texture2D(F_Sampler, next).rgb;
	for (int i = 0; i < 3; ++i)
	{
		next += full_offset;
		start += texture2D(F_Sampler, next).rgb;
	}
	vec3 color = F_Color.xyz * start / 5;
	if (F_Light_Enabled == 1)
	{
		if (F_Light_Power == 0)
		{
			gl_FragColor = vec4(vec3(F_Ambient, F_Ambient, F_Ambient) * color, F_Color.a);
		}
		else
		{
			float light_distance_squared = F_To_Light.x * F_To_Light.x + F_To_Light.y * F_To_Light.y + F_To_Light.z * F_To_Light.z;
			float cosine_alpha;
			float cosine_theta = dot(F_Normal, normalize(F_To_Light.xyz));
			if (cosine_theta < 0 || cosine_theta > 1)
			{
				cosine_theta = dot(-1 * F_Normal, normalize(F_To_Light.xyz));
				cosine_alpha = clamp(dot(normalize(F_To_Camera), reflect(-1 * F_To_Light.xyz, -1 * F_Normal)), 0,1);
			}
			else
			{
				cosine_alpha = clamp(dot(normalize(F_To_Camera), reflect(-1 * F_To_Light.xyz, F_Normal)), 0,1);
			}
			gl_FragColor = vec4(
				// Ambient
				vec3(F_Ambient, F_Ambient, F_Ambient) * color
				// Diffuse
				+ (F_To_Light.w == 1 ? F_Light_Color * color * F_Light_Power * cosine_theta / light_distance_squared : F_Light_Color * color * cosine_theta)
				// Specular
				+ (F_To_Light.w == 1 ? F_Light_Color * F_Light_Power * pow(cosine_alpha, F_Shininess) / light_distance_squared : F_Light_Color * pow(cosine_alpha, F_Shininess))
				// Alpha
				, F_Color.a);
		}
	}
	else
	{
		gl_FragColor = vec4(color, F_Color.a);
	}
}
