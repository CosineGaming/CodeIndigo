// Allows for customization of lighting

#include "GL/glew.h"

#include "Indigo.h"
#include "Lighting.h"

#include "GLFW/glfw3.h"
#include <iostream>


Lighting::Lighting(void)
{
	position = glm::vec4(0, 0, 0, 0);
	color = glm::vec3(0, 0, 0);
	ambient = 0.075;
	light_power = 0;
	return;
}


Lighting::Lighting(const Lighting& arrangement)
{
	position = arrangement.position;
	color = arrangement.color;
	ambient = arrangement.ambient;
	light_power = arrangement.light_power;
	return;
}


Lighting::~Lighting(void)
{
	return;
}


void Lighting::Set_Ambient(float intensity)
{
	ambient = intensity;
	return;
}


void Lighting::Set_Light(float X, float Y, float Z, bool direction,
	float power, glm::vec3 light_color)
{
	position = glm::vec4(X, Y, Z, 1.0 - (float) direction);
	light_power = power;
	color = light_color;
	return;
}


// Update the positions of all lights, and ensure they're enabled
void Lighting::Update_Lights(void) const
{
	// Light position
	glUniform1f(Indigo::Current_World.Shader_Location("F_Ambient", true), ambient);
	glUniform1f(Indigo::Current_World.Shader_Location("F_Light_Power", true), light_power);
	glUniform3f(Indigo::Current_World.Shader_Location("F_Light_Color", true), color[0], color[1], color[2]);
	glUniform4f(Indigo::Current_World.Shader_Location("Light", true), position[0], position[1], position[2], position[3]);
}
