// Entry point!
// Code indigo is a 3d mystery game. Readme for more information.

#include "CodeIndigo.h"
#include <iostream>

int object;

int eyeMark;

int fun;


void pressed (unsigned char key, int x, int y)
{
	if (key == '2')
	{
		Indigo::Current_World.Get_Object (object).Line = !Indigo::Current_World.Get_Object (object).Line;
	}
	return;
}

void sphere (const int& frame, Object& self)
{
	
	float X = 0;
	float Z = 0;

	if (Indigo::keys ['l'])
		X += 0.05;
	if (Indigo::keys ['j'])
		X -= 0.05;
	if (Indigo::keys ['i'])
		Z -= 0.05;
	if (Indigo::keys ['k'])
		Z += 0.05;

	if (!(self.Collide (Indigo::Current_World.Get_Object (fun), X, 0, Z)))
	{
		self.X += X;
		self.Z += Z;
	}

}

void update (int frame)
{

	Camera * camera = &Indigo::Current_World.camera;

	if (Indigo::keys ['w'])
	{
		camera->Move (0.05);
	}
	if (Indigo::keys ['s'])
	{
		camera->Move (-0.05);
	}
	if (Indigo::keys ['a'])
	{
		camera->Move (0.0, 0.05);
	}
	if (Indigo::keys ['d'])
	{
		camera->Move (0.0, -0.05);
	}
	if (Indigo::keys ['q'])
	{
		camera->Move (0.0, 0.0, -0.05);
	}
	if (Indigo::keys ['e'])
	{
		camera->Move (0.0, 0.0, 0.05);
	}

	if (Indigo::keys ['3'])
	{
		std::cout << camera->X << ", " << camera->Y << ", " << camera->Z << " looking at "
			<< camera->eye.Get_X () << ", " << camera->eye.Get_Y () << ", " << camera->eye.Get_Z () << std::endl;
	}

	if (Indigo::keys ['`'])
	{
		exit(0);
	}

	if (GL_NO_ERROR != glGetError ())
	{
		std::cout << "\aError: " << glGetError () << std::endl;
	}

}

void mouse_moved (int x, int y)
{

	Camera * camera = &Indigo::Current_World.camera;

	camera->eye.Normalize ();

	static const float sensitivity = 1;

	float y_angle = camera->eye.Get_Y_Angle () + y * -1 * sensitivity;

	if (!(y_angle >= 89.9 && y_angle <= 270.1))
	{
		camera->eye.Add_Direction (0.0, x * -1 * sensitivity,
			y * -1 * sensitivity);
	}
	else
	{
		if (y_angle < 180)
		{
			camera->eye.Set_Direction (1.0, camera->eye.Get_X_Angle () + x * -1 * sensitivity, 89.9);
		}
		else
		{
			camera->eye.Set_Direction (1.0, camera->eye.Get_X_Angle () + x * -1 * sensitivity, 270.1);
		}
	}

}

int main(int argc, char ** argv)
{
	Direction test (20.0, 90.0, 0.0);
	test.Add_Direction (20.0, 90.0, 0.0);
	test.Normalize ();
	std::cout << (int) test.Get_X () << std::endl;
	std::cout << (int) test.Get_Y () << std::endl;
	std::cout << (int) test.Get_Z () << std::endl;
	Indigo::Initialize (argc, argv, "Code Indigo");
	Indigo::Current_World.Add_Object (Object(0.0, 1.0, 0.0, Mesh::Sphere(0.2, 2)));
	Indigo::Current_World.Add_Object (Object (1.0, 0.5, -1.0, Mesh::Cube (0.5), 
		Indigo::White_Color, 60.0, nullptr, false));
	Indigo::Current_World.Add_Object (Object (0.0, 0.0, 0.0, Mesh::Cube (2.0)));
	Indigo::Update_Function = update;
	Indigo::Key_Pressed_Function = pressed;
	Indigo::Relative_Mouse_Moved_Function = mouse_moved;
	Indigo::Current_World.lighting.Add_Light (0.0, 1.0, 10.0);
	Indigo::Current_World.camera.Place (0.0, 0.0, 0.0);
	Indigo::Run ();
	return (0);
}