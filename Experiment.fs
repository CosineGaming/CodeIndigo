#version 120

varying vec2 F_UV;
varying vec3 F_Normal;
varying vec3 F_To_Camera;
varying vec4 F_To_Light;

uniform vec3 F_Color;
uniform sampler2D F_Sampler;
uniform float F_Light_Power;
uniform float F_Ambient;
uniform float F_Shininess;
uniform vec3 F_Light_Color;
uniform int F_Light_Enabled;

// Automatic: out vec4 gl_FragColor;

int rand(int seed)
{
	// For lack of quick XOR, just do weird (fast) shit
	return seed*seed*seed*seed;
}

void main()
{
	vec3 color = 0.9 * F_Color * texture2D(F_Sampler, vec2(int(F_UV.x / 0.04) * 0.04, int(F_UV.y / 0.04) * 0.04)).rgb;
	int x_rand = rand(int(F_Normal.x * 235544345));
	int y_rand = rand(int(F_Normal.y * 254556478));
	int z_rand = rand(int(F_Normal.z * 245654678));
	color += 0.1 * vec3((x_rand - x_rand / 1000 * 1000) / 1000.0, (y_rand - y_rand / 1000 * 1000) / 1000.0, (z_rand - z_rand / 1000 * 1000) / 1000.0);
	if (F_Light_Enabled == 1)
	{
		float light_distance_squared = F_To_Light.x * F_To_Light.x + F_To_Light.y * F_To_Light.y + F_To_Light.z * F_To_Light.z;
		float cosine_theta = abs(dot(F_Normal, normalize(F_To_Light.xyz)));
		float cosine_alpha = clamp(dot(normalize(F_To_Camera), reflect(-1 * F_To_Light.xyz, F_Normal)), 0, 1);
		gl_FragColor = vec4(
			// Ambient
			vec3(F_Ambient, F_Ambient, F_Ambient) * color
			// Diffuse
			+ (F_To_Light.w == 1 ? F_Light_Color * color * F_Light_Power * cosine_theta / light_distance_squared : F_Light_Color * color * cosine_theta)
			// Specular
			+ (F_To_Light.w == 1 ? F_Light_Color * F_Light_Power * pow(cosine_alpha, F_Shininess) / light_distance_squared : F_Light_Color * pow(cosine_alpha, F_Shininess))
			// Alpha
			, 1);
	}
	else
	{
		gl_FragColor = vec4(color, 1); // Uncomment for debug
	}
}
