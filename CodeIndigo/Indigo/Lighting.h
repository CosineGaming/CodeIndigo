// Allows for customization of lighting

#pragma once


class Lighting
{

public:

	// Create a new, blank light arrangement
	Lighting(void);
	// Copy a light arrangement
	Lighting(const Lighting& arrangement);
	// Destroy a lighting arrangement, reset to default
	~Lighting(void);
	// Set ambient lighting for the scene for easier lighting
	void Set_Ambient(float intensity);
	// Set the light source
	void Set_Light(float X, float Y, float Z, bool direction=false,
		float power=60.0, glm::vec3 light_color=glm::vec3(1.0,1.0,1.0));
	// Send the light to the shaders
	void Update_Lights(void) const;

private:

	// The position of the lights for arranging
	glm::vec4 position;
	// The color of the light, usually full white
	glm::vec3 color;
	// The stored ambient value for update
	float ambient;
	// The power of the light -- how far it stretches
	float light_power;

};