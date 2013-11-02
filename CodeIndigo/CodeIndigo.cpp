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
		camera->Move (0.0, -0.05);
	}
	if (Indigo::keys ['d'])
	{
		camera->Move (0.0, 0.05);
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

	if (GL_NO_ERROR != glGetError ())
	{
		std::cout << "\aError: " << glGetError () << std::endl;
	}

}

void mouse_moved (int x, int y)
{

	std::cout << x << ", " << y << std::endl;

	Camera * camera = &Indigo::Current_World.camera;

	camera->eye.Normalize ();

	static const float sensitivity = 1;

	float y_angle = camera->eye.Get_Y_Angle () + y * -1 * sensitivity;

	if (!(y_angle >= 89.9 && y_angle <= 270.1))
	{
		camera->eye.Add_Direction (0.0, x * sensitivity,
			y * -1 * sensitivity);
	}
	else
	{
		if (y_angle < 180)
		{
			camera->eye.Set_Direction (1.0, camera->eye.Get_X_Angle () + x * sensitivity, 89.9);
		}
		else
		{
			camera->eye.Set_Direction (1.0, camera->eye.Get_X_Angle () + x * sensitivity, 270.1);
		}
	}
	Indigo::Current_World.Get_Object (eyeMark).X = camera->X; // <DELETE>
	Indigo::Current_World.Get_Object (eyeMark).Y = camera->Y;
	Indigo::Current_World.Get_Object (eyeMark).Z = camera->Z;
	std::cout << camera->eye.Get_X_Angle () << "," << camera->eye.Get_Y_Angle () << "," << camera->eye.Get_Distance () << std::endl; // </DELETE>

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
	Mesh circle = Mesh::Sphere (0.5, 1);
	for (int Point=0; Point<circle.Size (); ++Point)
	{
		//std::cout << "{" << circle [Point].X << ", " << circle [Point].Y << ", " << circle [Point].Z << "},\n";
	}
	Mesh shape = 
		Vertex (0, 0, 0.5)
		+ Vertex (0.5, 0,0)
		+ Vertex (-0, 0.5,0)
		+ Vertex (0.5, 0,0)
		+ Vertex (0, 0, -0.5)
		+ Vertex (-0, 0.5,0)
		+ Vertex (0, 0, -0.5)
		+ Vertex (-0.5, 0, 0)
		+ Vertex (-0, 0.5,0)
		+ Vertex (-0.5, 0, 0)
		+ Vertex (0, 0, 0.5)
		+ Vertex (-0, 0.5,0)
		+ Vertex (0, 0, 0.5)
		+ Vertex (0.5, 0,0)
		+ Vertex (0, -0.5, 0)
		+ Vertex (0.5, 0,0)
		+ Vertex (0, 0, -0.5)
		+ Vertex (0, -0.5, 0)
		+ Vertex (0, 0, -0.5)
		+ Vertex (-0.5, 0, 0)
		+ Vertex (0, -0.5, 0)
		+ Vertex (-0.5, 0, 0)
		+ Vertex (0, 0, 0.5)
		+ Vertex (0, -0.5, 0);
	Mesh box = Mesh::Sphere (0.5, 1);
	Object add = Object (0.0, 0.0, -1.0, shape,
		Indigo::White_Color, 40.0f, sphere, true);
	object = Indigo::Current_World.Add_Object (add);
	Mesh wall = Mesh::Cube (0.5);
	Object thewall = Object (1.0, 0.5, -1.0, wall, 
		Indigo::White_Color, 60.0, nullptr, false);
	Object marker = Object (0.0, 0.0, 0.0, Mesh::Sphere (0.05, 1), Indigo::White_Color);
	eyeMark = Indigo::Current_World.Add_Object (marker);
	Object cage = Object (0.0, 0.0, 0.0, Mesh::Cube (2.0));
	//Indigo::Current_World.Add_Object (cage);
	fun = Indigo::Current_World.Add_Object (thewall);
	Indigo::Update_Function = update;
	Indigo::Key_Pressed_Function = pressed;
	Indigo::Relative_Mouse_Moved_Function = mouse_moved;
	Indigo::Current_World.lighting.Add_Light (0.0, 1.0, 10.0);
	Indigo::Current_World.camera.Place (0.0, 0.0, 0.0);
	Indigo::Run ();
	return (0);
}