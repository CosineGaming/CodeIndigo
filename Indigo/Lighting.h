// Allows for customization of Lighting

#pragma once

#include "glm/glm.hpp"
#include "glm/vec3.hpp"


class Lighting
{

public:

	// Create a new, blank Lighting arrangement
	Lighting(void);
	// Copy a Lighting arrangement
	Lighting(const Lighting& arrangement);
	// Destroy a Lighting arrangement, reset to default
	~Lighting(void);
	// Add a light that emits in all directions. Returns an ID
	int Add_Bulb(float X, float Y, float Z, float power = 60, glm::vec3 color = glm::vec3(1.0, 1.0, 1.0));
	// Add a light that emits in one direction. Returns an ID
	int Add_Sun(float X, float Y, float Z, float power = 1, glm::vec3 color = glm::vec3(1.0, 1.0, 1.0));
	// Add a light that shines in one direction. Returns an ID
	int Add_Lamp(float X, float Y, float Z, glm::vec3 direction, float lighted_angle = 180, float power = 60, glm::vec3 color = glm::vec3(1.0, 1.0, 1.0));
	// Remove a light, given an ID
	void Remove_Light(int ID);
	// Set the shader uniforms for the frame
	void Update_Lights(void) const;

private:

	// The position of each light
	glm::vec4 positions[8];
	// The color of each light, usually full white
	glm::vec3 colors[8];

	// The direction for lamps. W is 0 if not enabled.
	glm::vec4 directions[8];
	// The full angle given light from each lamp.
	float angles[8];

	// The number of lights that have actually been dedicated
	int number_of_lights;

};
