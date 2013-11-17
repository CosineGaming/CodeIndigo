// Entry point!
// Code indigo is a 3d mystery game. Readme for more information.

#include "CodeIndigo.h"
#include <iostream>

int bounds;

void update (int frame)
{

	Camera * camera = &Indigo::Current_World.camera;

	static float speed;
	speed = 0.05;

	static float gravity = 0;

	if (Indigo::keys ['\\'])
	{
		speed = 1.5;
	}
	if (Indigo::keys ['w'])
	{
		camera->Move (speed);
	}
	if (Indigo::keys ['s'])
	{
		camera->Move (-speed);
	}
	if (Indigo::keys ['a'])
	{
		camera->Move (0.0, speed);
	}
	if (Indigo::keys ['d'])
	{
		camera->Move (0.0, -speed);
	}
	if (Indigo::keys ['q'])
	{
		camera->Move (0.0, 0.0, -speed);
	}
	if (Indigo::keys ['e'])
	{
		camera->Move (0.0, 0.0, speed);
	}
	
	if (Indigo::keys [' '] && !Indigo::Current_World.Get_Object (bounds).CollideVertex (Vertex (camera->X, camera->Y - 0.6, camera->Z)))
	{
		gravity = 0.45;
	}
	gravity -= 0.05;
	if (Indigo::Current_World.Get_Object (bounds).CollideVertex (Vertex (camera->X, camera->Y - 0.6, camera->Z)) || gravity > 0)
	{
		camera->Move (0.0, 0.0, gravity);
	}
	else
	{
		gravity = 0;
		camera->Y = -1.4;
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

	static const float sensitivity = 0.5;

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
	Indigo::Current_World.Add_Object (Object (1.0, 0.5, -1.0, Mesh::Cube (0.5), Indigo::White_Color));
	bounds = Indigo::Current_World.Add_Object (Object (0.0, 0.0, 0.0, Mesh::Cube (4.0)));
	Indigo::Update_Function = update;
	Indigo::Relative_Mouse_Moved_Function = mouse_moved;
	Indigo::Current_World.lighting.Add_Light (0.0, 1.0, 10.0);
	Indigo::Current_World.camera.Place (0.0, 0.0, 0.0);
	Indigo::Run ();
	return (0);
}