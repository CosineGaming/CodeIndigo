// Allows for customization of lighting

#pragma once


class Lighting
{
public:
	// Create a new, blank light arrangement
	Lighting (void);
	// Copy a light arrangement
	Lighting (const Lighting& arrangement);
	// Destroy a lighting arrangement, reset to default
	~Lighting (void);
	// Add a light source
	void Add_Light (float X, float Y, float Z, bool infinity=false,
		float ambient=0.2, float specular=1.0, float * color_offset=nullptr);
	// Update the positions of all lights
	void Position_Lights (void) const;
	// Get the number of lights in the scene
	int Get_Number_Of_Lights (void) const;
private:
	// The OpenGL light values
	static const int Light_Values [8];
	// The position of the lights for arranging
	float * Light_Positions [8];
	// The number of lights in the current scene
	int Number_Of_Lights;
};