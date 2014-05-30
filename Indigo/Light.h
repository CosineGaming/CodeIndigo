// Allows for customization of Light

#pragma once

#include "glm/glm.hpp"
#include "glm/vec3.hpp"


class Light
{

public:

	// Create a new, blank light arrangement
	Light(void);
	// Copy a light arrangement
	Light(const Light& arrangement);
	// Destroy a Light arrangement, reset to default
	~Light(void);
	// Returns a light that emits in all directions.
	static Light Bulb(float X, float Y, float Z, bool coordinate = true, float power = 60, glm::vec3 color = glm::vec3(1.0, 1.0, 1.0));
	// Returns a light that shines in one direction
	static Light Lamp(float X, float Y, float Z, glm::vec3 direction, float lighted_angle = 180, float power = 60, glm::vec3 color = glm::vec3(1.0, 1.0, 1.0));

private:

	// The position of the lights for arranging
	glm::vec4 position;
	// The direction for lamps. W is 0 if not enabled.
	glm::vec4 pointing;
	// The full angle given light from a lamp.
	float angle;
	// The color of the light, usually full white
	glm::vec3 color;

};
