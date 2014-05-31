// Allows for customization of Lighting

#include "GL/glew.h"

#include "Indigo.h"
#include "Lighting.h"

#include "GLFW/glfw3.h"
#include <iostream>


Lighting::Lighting(void)
{
	for (int i = 0; i < 8; ++i)
	{
		Remove_Light(i);
	}
	number_of_lights = 0;
	return;
}


Lighting::Lighting(const Lighting& lighting) : Lighting()
{
	for (int i = 0; i < lighting.number_of_lights; ++i)
	{
		positions[i] = lighting.positions[i];
		colors[i] = lighting.colors[i];
		directions[i] = lighting.directions[i];
		angles[i] = lighting.angles[i];
	}
	number_of_lights = lighting.number_of_lights;
	return;
}


Lighting::~Lighting(void)
{
	return;
}


// Add a light that emits in all directions
int Lighting::Add_Bulb(float X, float Y, float Z, float power, glm::vec3 color)
{
	positions[number_of_lights] = glm::vec4(X, Y, Z, 1);
	colors[number_of_lights] = color * power;
	directions[number_of_lights] = glm::vec4(0, 0, 0, 0);
	angles[number_of_lights] = 0;
	return number_of_lights++;
}


// Add a light that emits in all directions
int Lighting::Add_Sun(float X, float Y, float Z, float power, glm::vec3 color)
{
	positions[number_of_lights] = glm::vec4(X, Y, Z, 0);
	colors[number_of_lights] = color * power;
	directions[number_of_lights] = glm::vec4(0, 0, 0, 0);
	angles[number_of_lights] = 0;
	return number_of_lights++;
}


// Add a light that shines in one direction. Returns an ID
int Lighting::Add_Lamp(float X, float Y, float Z, glm::vec3 direction, float lighted_angle, float power, glm::vec3 color)
{
	positions[number_of_lights] = glm::vec4(X, Y, Z, 1);
	colors[number_of_lights] = color * power;
	directions[number_of_lights] = glm::normalize(glm::vec4(direction, 1));
	angles[number_of_lights] = 180 / lighted_angle;
	return number_of_lights++;
}


// Remove a light, given an ID
void Lighting::Remove_Light(int ID)
{
	positions[ID] = glm::vec4(0, 0, 0, 1);
	colors[ID] = glm::vec3(0, 0, 0);
	directions[ID] = glm::vec4(0, 0, 0, 0);
	angles[ID] = 0;
}


// Set the shader uniforms for the frame
void Lighting::Update_Lights(void) const
{
	glUniform4fv(Indigo::Current_World.Shader_Location("V_Lights", true), 8, &positions[0][0]);
	glUniform3fv(Indigo::Current_World.Shader_Location("F_Light_Colors", true), 8, &colors[0][0]);
	glUniform4fv(Indigo::Current_World.Shader_Location("F_N_Light_Directions", true), 8, &directions[0][0]);
	glUniform1fv(Indigo::Current_World.Shader_Location("F_Light_Angles", true), 8, &angles[0]);
	glUniform1i(Indigo::Current_World.Shader_Location("F_Number_Of_Lights", true), number_of_lights);
}
