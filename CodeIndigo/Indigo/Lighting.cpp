// Allows for customization of lighting

#include "Lighting.h"
#include <stdlib.h>
#include "glut.h"
#include <iostream>


Lighting::Lighting(void)
{
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	Set_Ambient(0.15);
	number_of_lights = 0;
	return;
}


Lighting::Lighting(const Lighting& arrangement)
{
	number_of_lights = arrangement.Get_Number_Of_Lights();
	for (int light = 0; light < number_of_lights; ++light)
	{
		for (int i = 0; i < 4; ++i)
		{
			light_positions[light][i] = arrangement.light_positions[light][i];
		}
	}
	return;
}


Lighting::~Lighting(void)
{
	return;
}


void Lighting::Set_Ambient(float intensity)
{
	float ambient[] = {intensity, intensity, intensity, 1.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
	return;
}


void Lighting::Add_Light(float X, float Y, float Z, bool infinity,
	float diffuse, float specular, float * color_offset)
{
	int Light = light_values[number_of_lights];
	float position_array[] = {X, Y, Z, 1.0 - (float) infinity};
	float diffuse_array [] = {diffuse, diffuse, diffuse, 1.0};
	float specular_array[] = {specular, specular, specular, 1.0};
	if (color_offset)
	{
		for (int Color = 0; Color<3; ++Color)
		{
			specular_array[Color] += color_offset[Color];
		}
	}
	light_positions[number_of_lights][0] = position_array[0];
	light_positions[number_of_lights][1] = position_array[1];
	light_positions[number_of_lights][2] = position_array[2];
	light_positions[number_of_lights][3] = position_array[3];
	glEnable(Light);
	for (int i=number_of_lights+1; i<8; ++i)
	{
		glDisable(light_values[i]);
	}
	glLightfv(Light, GL_POSITION, position_array);
	glLightfv(Light, GL_DIFFUSE, diffuse_array);
	glLightfv(Light, GL_SPECULAR, specular_array);
	++number_of_lights;
	return;
}


// Update the positions of all lights, and ensure they're enabled
void Lighting::Update_Lights(void) const
{
	for (int Light = 0; Light<number_of_lights; ++Light)
	{
		glLightfv(light_values[Light], GL_POSITION, light_positions[Light]);
	}
}


int Lighting::Get_Number_Of_Lights(void) const
{
	return (number_of_lights);
}


const int Lighting::light_values[8] = {GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3,
	GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7};
