// Allows for customization of lighting

#include "Lighting.h"
#include <stdlib.h>
#include "glut.h"
#include <iostream>


Lighting::Lighting(void)
{
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
	ambient[0] = intensity;
	ambient[1] = intensity;
	ambient[2] = intensity;
	ambient[3] = 1.0;
	return;
}


void Lighting::Add_Light(float X, float Y, float Z, bool direction,
	float diffuse, float specular, float * color_offset)
{
	int light = light_values[number_of_lights];
	float position_array[] = {X, Y, Z, 1.0 - (float) direction};
	float diffuse_array [] = {diffuse, diffuse, diffuse, 1.0};
	float specular_array[] = {specular, specular, specular, 1.0};
	if (color_offset)
	{
		for (int color = 0; color<3; ++color)
		{
			specular_array[color] += color_offset[color];
		}
	}
	light_positions[number_of_lights][0] = position_array[0];
	light_positions[number_of_lights][1] = position_array[1];
	light_positions[number_of_lights][2] = position_array[2];
	light_positions[number_of_lights][3] = position_array[3];
	glEnable(light);
	for (int i=number_of_lights+1; i<8; ++i)
	{
		glDisable(light_values[i]);
	}
	glLightfv(light, GL_POSITION, position_array);
	glLightfv(light, GL_DIFFUSE, diffuse_array);
	glLightfv(light, GL_SPECULAR, specular_array);
	++number_of_lights;
	return;
}


// Update the positions of all lights, and ensure they're enabled
void Lighting::Update_Lights(void) const
{
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
	for (int light = 0; light<number_of_lights; ++light)
	{
		glLightfv(light_values[light], GL_POSITION, light_positions[light]);
	}
}


int Lighting::Get_Number_Of_Lights(void) const
{
	return (number_of_lights);
}


const int Lighting::light_values[8] = {GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3,
	GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7};
