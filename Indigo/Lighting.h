// Allows for customization of Lighting

#pragma once

#include "glm/glm.hpp"
#include "glm/vec3.hpp"

class World;


class Lighting
{

public:

	// Create a new, blank Lighting arrangement
	Lighting(void);
	// Copy a Lighting arrangement
	Lighting(const Lighting& arrangement);
	// Destroy a Lighting arrangement, reset to default
	~Lighting(void);
	// Set the lighting for the scene that is applied to all areas
	void Set_Ambient(float power = 0, glm::vec3 color = glm::vec3(1, 1, 1));
	// Add a light that emits in all directions. Returns an ID
	int Add_Bulb(float X, float Y, float Z, float power = 1, glm::vec3 color = glm::vec3(1, 1, 1));
	// Add a light that emits in one direction. Returns an ID
	int Add_Sun(float X, float Y, float Z, float power = 1, glm::vec3 color = glm::vec3(1, 1, 1));
	// Add a light that shines in one direction. Returns an ID
	int Add_Lamp(float X, float Y, float Z, glm::vec3 direction, float lighted_angle = 180, float power = 1, glm::vec3 color = glm::vec3(1, 1, 1));
	// Remove a light, given an ID
	void Remove_Light(int ID);
	// Set the shader uniforms for the frame
	void Update_Lights(const World& holder, const glm::mat4& view) const;

private:

	// The position of each light
	glm::vec4 positions[8];
	// The color of each light, usually full white
	glm::vec3 colors[8];

	// The ambient value applied to all fragments
	glm::vec3 ambient;

	// The direction for lamps. W is 1 if not enabled.
	glm::vec4 directions[8];
	// The full angle given light from each lamp.
	float angles[8];

	// The number of lights that have actually been dedicated
	int number_of_lights;

};
