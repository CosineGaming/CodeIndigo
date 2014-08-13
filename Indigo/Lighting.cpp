// Allows for customization of Lighting

#include "GL/glew.h"

#include "Lighting.h"

#include "Indigo.h"
#include "Camera.h"
#include "World.h"

#include "GLFW/glfw3.h"
#include <iostream>
#include "glm/vec3.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"


Lighting::Lighting(void)
{
	for (int i = 0; i < 8; ++i)
	{
		Remove_Light(i);
	}
	Set_Ambient(0);
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


// Set the lighting for the scene that is applied to all areas
void Lighting::Set_Ambient(float power, glm::vec3 color)
{
	ambient = color * power;
}


// Add a light that emits in all directions
int Lighting::Add_Bulb(float X, float Y, float Z, float power, glm::vec3 color)
{
	positions[number_of_lights] = glm::vec4(X, Y, Z, 1);
	colors[number_of_lights] = color * power;
	directions[number_of_lights] = glm::vec4(0, 0, 0, 1);
	angles[number_of_lights] = 0;
	return number_of_lights++;
}


// Add a light that emits in all directions
int Lighting::Add_Sun(float X, float Y, float Z, float power, glm::vec3 color)
{
	positions[number_of_lights] = glm::vec4(X, Y, Z, 0);
	colors[number_of_lights] = color * power;
	directions[number_of_lights] = glm::vec4(0, 0, 0, 1);
	angles[number_of_lights] = 0;
	shadow_matrices[number_of_lights] = glm::mat4(glm::ortho(-10, 10, -10, 10, -10, 20)) * glm::mat4(glm::lookAt(glm::vec3(-1 * X, -1 * Y, -1 * Z), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
	return number_of_lights++;
}


// Add a light that shines in one direction. Returns an ID
int Lighting::Add_Lamp(float X, float Y, float Z, glm::vec3 direction, float lighted_angle, float power, glm::vec3 color)
{
	positions[number_of_lights] = glm::vec4(X, Y, Z, 1);
	colors[number_of_lights] = color * power;
	directions[number_of_lights] = glm::vec4(direction, 0);
	angles[number_of_lights] = 180 / lighted_angle;
	return number_of_lights++;
}


// Remove a light, given an ID
void Lighting::Remove_Light(int ID)
{
	positions[ID] = glm::vec4(0, 0, 0, 1);
	colors[ID] = glm::vec3(0, 0, 0);
	directions[ID] = glm::vec4(0, 0, 0, 1);
	angles[ID] = 0;
}


// Set the shader uniforms for the frame
void Lighting::Update_Lights(Camera& camera, const glm::mat4& view) const
{
	glUniform4fv(camera.Shader_Location("V_Lights", true), 8, &positions[0][0]);
	glUniform3fv(camera.Shader_Location("F_Light_Colors", true), 8, &colors[0][0]);
	glm::vec4 world_directions[8];
	for (int i = 0; i < number_of_lights; ++i)
	{
		world_directions[i] = directions[i];
		if (directions[i].w < 0.5)
		{
			world_directions[i] = view * world_directions[i];
		}
	}
	glUniform4fv(camera.Shader_Location("V_Lamp_Directions", true), 8, &world_directions[0][0]);
	glUniform1fv(camera.Shader_Location("F_Lamp_Angles", true), 8, &angles[0]);
	glUniform3f(camera.Shader_Location("F_Ambient", true), ambient.r, ambient.g, ambient.b);
	glUniform1i(camera.Shader_Location("F_Number_Of_Lights", true), number_of_lights);
}
