#version 330 core

varying vec2 F_UV;
varying vec3 F_Normal;
varying vec3 F_To_Camera;
varying vec4 F_To_Light;

uniform vec3 F_Color;
uniform sampler2D F_Sampler;

// Automatic: out vec4 gl_FragColor;

int rand(int seed)
{
	// For lack of quick XOR, just do weird (fast) shit
	int result = (seed*seed + seed)*seed;
	return result;
}

void main()
{
	vec3 color = 0.7 * texture2D(F_Sampler, vec2(int(F_UV.x / 0.04) * 0.04, int(F_UV.y / 0.04) * 0.04)).rgb;
	int x_rand = rand(int(F_Normal.x * 2354345));
	int y_rand = rand(int(F_Normal.y * 2545478));
	int z_rand = rand(int(F_Normal.z * 2456678));
	color += 0.3 * vec3((x_rand - x_rand / 1000 * 1000) / 1000.0, (y_rand - y_rand / 1000 * 1000) / 1000.0, (z_rand - z_rand / 1000 * 1000) / 1000.0);
	float light_distance_squared = F_To_Light.x * F_To_Light.x + F_To_Light.y * F_To_Light.y + F_To_Light.z * F_To_Light.z;
	float cosine_theta = abs(dot(F_Normal, normalize(F_To_Light.xyz)));
	float cosine_alpha = clamp(dot(normalize(F_To_Camera), reflect(-1 * F_To_Light.xyz, F_Normal)), 0, 1);
	float F_Shininess = 10; // Todo: uniform
	float F_Light_Power = 1200; // Todo: uniform
	gl_FragColor = vec4(
		// Ambient
		vec3(0.00, 0.00, 0.00) * color
		// Diffuse
		+ (F_To_Light.w == 1 ? color * F_Light_Power * cosine_theta / light_distance_squared : color * cosine_theta)
		// Specular
		+ (F_To_Light.w == 1 ? F_Light_Power * pow(cosine_alpha, F_Shininess) / light_distance_squared : pow(cosine_alpha, F_Shininess))
		// Alpha
		, 1);
}
