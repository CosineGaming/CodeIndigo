// Entry point!
// Code indigo is a 3d mystery game. Readme for more information.

#include "CodeIndigo.h"
#include <iostream>

int bounds;

void update (int frame)
{

	Camera * camera = &Indigo::Current_World.camera;

	static float speed;
	int part_frame = frame % 30;
	if (part_frame > 15)
	{
		part_frame = 30 - part_frame;
	}
	speed = 0.001 * part_frame + 0.016;

	static float gravity = 0;

	if (Indigo::keys ['\\'])
	{
		speed += 1.5;
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
	
	if (Indigo::keys [' '] && !Indigo::Current_World.Get_Object (bounds).Collide_Vertex (Vertex (camera->X, camera->Y - 1.0, camera->Z)))
	{
		gravity = 0.256;
	}
	gravity -= 0.1635;
	if (Indigo::Current_World.Get_Object (bounds).Collide_Vertex (Vertex (camera->X, camera->Y - 1.0, camera->Z)) || gravity > 0)
	{
		camera->Move (0.0, 0.0, gravity);
	}
	else
	{
		gravity = 0;
		camera->Y = -1.5;
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

	static const float sensitivity = 0.2;

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
	Indigo::Current_World.Add_Object (Object(0.0, 0.05, 0.0, Mesh::Sphere(0.3, 3), Indigo::Blue_Color));
	Indigo::Current_World.Add_Object (Object (1.0, -1.25, -1.0, Mesh::Cube (0.5), Indigo::White_Color));
	bounds = Indigo::Current_World.Add_Object (Object (0.0, 0.0, 0.0, Mesh::Cube (4.5)));
	Indigo::Update_Function = update;
	Indigo::Relative_Mouse_Moved_Function = mouse_moved;
	Indigo::Current_World.lighting.Add_Light (0.0, 1.0, 10.0);
	Indigo::Current_World.camera.Place (0.0, 0.0, 0.0);
	Indigo::Run ();
	return (0);
}
