// Entry point!
// Code indigo is a 3d mystery game. Readme for more information.

#include "CodeIndigo.h"
#include <iostream>

int object;


void pressed (unsigned char key, int x, int y)
{
	if (key == '2')
	{
		Indigo::Current_World.Get_Object (object).Line = !Indigo::Current_World.Get_Object (object).Line;
	}
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
	if (Indigo::keys ['q'])
	{
		Indigo::Current_World.camera.Move (0.0, 0.0, -0.05);
	}
	if (Indigo::keys ['e'])
	{
		Indigo::Current_World.camera.Move (0.0, 0.0, 0.05);
	}
	if (Indigo::keys ['3'])
	{
		Camera camera = Indigo::Current_World.camera;
		std::cout << camera.X << ", " << camera.Y << ", " << camera.Z << " looking at "
			<< camera.eye.Get_X () << ", " << camera.eye.Get_Y () << ", " << camera.eye.Get_Z () << std::endl;
	}
	if (GL_NO_ERROR != glGetError ())
	{
		std::cout << "Error: " << glGetError () << std::endl;
	}
}

void mouse_moved (int x, int y)
{
	static const float sensitivity = 0.5;
	Indigo::Current_World.camera.eye.Add_Direction (0.0, x * sensitivity,
		y * -1 * sensitivity);
}

int main(int argc, char ** argv)
{
	Indigo::Initialize (argc, argv, " ~ Code Indigo",
		800, 600, true, 60, nullptr, 60);
	Mesh box = Mesh::Sphere (0.5);
	Object add = Object (0.0, 0.0, -1.0, box,
		Indigo::White_Color, 40.0f, nullptr, true);
	object = Indigo::Current_World.Add_Object (add);
	Mesh wall = Mesh::Cube (0.5);
	Object thewall = Object (1.0, 0.5, -1.0, wall,
		Indigo::White_Color, 60.0f, nullptr, false);
	Indigo::Current_World.Add_Object (thewall);
	Indigo::Update_Function = update;
	Indigo::Key_Pressed_Function = pressed;
	Indigo::Relative_Mouse_Moved_Function = mouse_moved;
	Indigo::Current_World.lighting.Add_Light (0.0, 1.0, 10.0);
	Indigo::Current_World.camera.Place (0.0, 0.0, 0.0);
	Indigo::Current_World.camera.eye.Set_Direction (1.0, 90.0, -2.8);
	Indigo::Run ();
	return (0);
}