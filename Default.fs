#version 120

varying vec2 F_UV;
varying vec3 F_Normal;
varying vec3 F_To_Camera;
varying vec3 F_To_Light;

uniform vec3 F_Color;
uniform sampler2D F_Sampler;

//out vec3 gl_FragColor;

void main()
{
	gl_FragColor = vec4(texture2D(F_Sampler, F_UV).rgb, 1);// * clamp(dot(normalize(F_Normal), normalize(F_To_Light)), 1, 1);
}
