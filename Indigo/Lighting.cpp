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
	ambient = 0; // .075;
	return;
}


Lighting::Lighting(const Lighting& arrangement)
{
	position = arrangement.position;
	color = arrangement.color;
	ambient = arrangement.ambient;
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


void Lighting::Set_Light(float X, float Y, float Z, bool coordinate,
	float power, glm::vec3 light_color)
{
	position = glm::vec4(X, Y, Z, coordinate);
	color = light_color * power;
	return;
}


// Update the positions of all lights, and ensure they're enabled
void Lighting::Update_Lights(void) const
{
	// Light position
	glUniform1f(Indigo::Current_World.Shader_Location("F_Ambient", true), ambient);
	glUniform3f(Indigo::Current_World.Shader_Location("F_Light_Color", true), color.r, color.g, color.b);
	glUniform4f(Indigo::Current_World.Shader_Location("V_Light", true), position.x, position.y, position.z, position.w);
}
