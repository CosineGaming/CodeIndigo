// Entry point!
// Code indigo is a 3d mystery game. Readme for more information.

#include "CodeIndigo.h"
#include <iostream>


void box_update (const int& frame, const Object& self)
{
	return;
}

void update (int frame)
{
	if (Indigo::keys ['w'])
	{
		Indigo::Current_World.camera.Move (0.05);
	}
	if (Indigo::keys ['s'])
	{
		Indigo::Current_World.camera.Move (-0.05);
	}
	if (Indigo::keys ['a'])
	{
		Indigo::Current_World.camera.Move (0.0, -0.05);
	}
	if (Indigo::keys ['d'])
	{
		Indigo::Current_World.camera.Move (0.0, 0.05);
	}
	if (GL_NO_ERROR != glGetError ())
	{
		std::cout << "Error: " << glGetError () << std::endl;
	}
}

int main(int argc, char ** argv)
{
	Indigo::Initialize (argc, argv, " ~ Code Indigo",
		800, 600, false, 60, Indigo::Sky_Color, 60);
	Mesh box = Mesh::Cube (0.75);
	Object add = Object(0.0, 0.0, -1.0, box, Indigo::White_Color, 40.0f, box_update);
	int object = Indigo::Current_World.Add_Object (add);
	Indigo::Update_Function = update;
	Indigo::Current_World.lighting.Add_Light (0.0, 2.0, 2.0);
	Direction distance (2.0, 180.0, 45.0);
	Indigo::Current_World.camera.Watch (Indigo::Current_World.Get_Object (object), distance);
	Indigo::Current_World.camera.Look_At (0.0, 0.0, -1.0);
	Indigo::Run ();
	return (0);
}