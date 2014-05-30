// Allows for customization of Light

#include "GL/glew.h"

#include "Indigo.h"
#include "Light.h"

#include "GLFW/glfw3.h"
#include <iostream>


Light::Light(void)
{
	position = glm::vec4(0, 0, 0, 0);
	color = glm::vec3(0, 0, 0);
	pointing = glm::vec4(0, 0, 0, 0);
	angle = 0;
	return;
}


Light::Light(const Light& light)
{
	position = light.position;
	color = light.color;
	pointing = light.pointing;
	angle = light.angle;
	return;
}


Light::~Light(void)
{
	return;
}


Light Light::Bulb(float X, float Y, float Z, bool coordinate, float power, glm::vec3 color)
{
	Light light;
	light.position = glm::vec4(X, Y, Z, coordinate);
	light.color = color * power;
	return light;
}


Light Light::Lamp(float X, float Y, float Z, glm::vec3 direction, float lighted_angle, float power, glm::vec3 color)
{
	Light light;
	light.position = glm::vec4(X, Y, Z, 1);
	light.pointing = glm::vec4(direction, 1);
	light.angle = 180 / lighted_angle;
	light.color = color * power;
	return light;
}
