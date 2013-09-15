// Allows for customization of lighting

#include "Lighting.h"
#include <stdlib.h>
#include "glut.h"


Lighting::Lighting (void)
{
	glLightModeli (GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glEnable (GL_LIGHTING);
	Number_Of_Lights = 0;
	return;
}


Lighting::Lighting (const Lighting& arrangement)
{
	glEnable (GL_LIGHTING);
	Number_Of_Lights = arrangement.Get_Number_Of_Lights ();
	return;
}


Lighting::~Lighting (void)
{
	glDisable (GL_LIGHTING);
	for (int Light_Index=0; Light_Index<Number_Of_Lights; ++Light_Index)
	{
		glDisable (Light_Values [Light_Index]);
	}
	return;
}


void Lighting::Add_Light (float X, float Y, float Z, bool infinity,
		float ambient, float specular, float * color_offset)
{
	int Light = Light_Values [Number_Of_Lights];
	glEnable (Light);
	float position_array [] = {X, Y, Z, (float)infinity};
	float ambient_array  [] = {ambient, ambient, ambient, 1.0};
	float diffuse_array  [] = {1.0 - ambient, 1.0 - ambient, 1.0 - ambient, 1.0};
	float specular_array [] = {specular, specular, specular, 1.0};
	if (color_offset)
	{
		for (int Color=0; Color<3; ++Color)
		{
			specular_array [Color] += color_offset [Color];
		}
	}
	Light_Positions [Number_Of_Lights] = position_array;
	glLightfv (Light, GL_POSITION, position_array);
	glLightfv (Light, GL_AMBIENT, ambient_array);
	glLightfv (Light, GL_DIFFUSE, diffuse_array);
	glLightfv (Light, GL_SPECULAR, specular_array);
	++Number_Of_Lights;
	return;
}


void Lighting::Position_Lights (void) const
{
	// Update the positions of all lights
	for (int Light=0; Light<Number_Of_Lights; ++Light)
	{
		glLightfv (Light_Values [Light], GL_POSITION, Light_Positions [Light]);
	}
}


int Lighting::Get_Number_Of_Lights (void) const
{
	return Number_Of_Lights;
}


const int Lighting::Light_Values [8] = {GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3,
	GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7};