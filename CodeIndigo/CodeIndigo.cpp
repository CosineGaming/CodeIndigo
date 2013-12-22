// Entry point!
// Code indigo is a 3d mystery game. Readme for more information.

#include "CodeIndigo.h"
#include <iostream>
#include "time.h"

int bounds;
int table;

void update(int time)
{

	Camera &camera = Indigo::Current_World.camera;

	float speed = .0015 * time;

	static float gravity = 0;

	if (Indigo::Shift)
	{
		speed = .003 * time;
	}
	if (Indigo::keys['e'])
	{
		speed += .01 * time;
	}

	if (Indigo::keys['w'])
	{
		camera.Move(speed);
	}
	if (Indigo::keys['s'])
	{
		camera.Move(-speed);
	}
	if (Indigo::keys['a'])
	{
		camera.Move(0.0, speed);
	}
	if (Indigo::keys['d'])
	{
		camera.Move(0.0, -speed);
	}
	if (Indigo::keys['r'])
	{
		camera.Place(0.0, 0.0, 0.0);
		gravity = 0;
	}

	if (Indigo::keys[' '] && !Indigo::Current_World.Collide(Vertex(camera.X, camera.Y - 1.5, camera.Z)))
	{
		gravity = 2.8;
	}

	if (Indigo::Current_World.Collide(Vertex(camera.X, camera.Y - 1.5, camera.Z)) || gravity > 0)
	{
		gravity -= .00980665 * time;
		if (!Indigo::keys['q'] && !Indigo::keys['f'])
			camera.Move(0.0, 0.0, gravity * time / 1000);
		if (Indigo::keys['f'])
			camera.Move(0.0, 0.0, gravity * time / -1000);
	}
	else
	{
		gravity = 0;
		camera.Y = 1.5;//int (camera.Y - 1.5) + 1.5;
	}

	if (Indigo::keys ['3'])
	{
		std::cout << "Position: " << camera.X << ", " << camera.Y << ", " << camera.Z << std::endl
			<< 1000 / time << " FPS" << std::endl << std::endl;
	}
	if (GL_NO_ERROR != glGetError())
	{
		std::cout << "\aError: " << glGetError() << std::endl;
	}
	if (Indigo::keys ['`'])
	{
		exit(0);
	}

}

void mouse_moved(int x, int y)
{

	Camera * camera = &Indigo::Current_World.camera;

	camera->eye.Normalize();

	static const float sensitivity = 0.2;

	float y_angle = camera->eye.Get_Y_Angle() + y * -1 * sensitivity;

	if (!(y_angle >= 89.9 && y_angle <= 270.1))
	{
		camera->eye.Add_Direction(0.0, x * -1 * sensitivity,
			y * -1 * sensitivity);
	}
	else
	{
		if (y_angle < 180)
		{
			camera->eye.Set_Direction(1.0, camera->eye.Get_X_Angle() + x * -1 * sensitivity, 89.9);
		}
		else
		{
			camera->eye.Set_Direction(1.0, camera->eye.Get_X_Angle() + x * -1 * sensitivity, 270.1);
		}
	}

}

int main(int argc, char ** argv)
{
	float color[3] = {0.0, 0.0, 0.0};
	Indigo::Initialize(argc, argv, "Code Indigo", 48, true);//, color);
	Object testies = Object(-1.0, 1.7, 0.0, Mesh::Sphere(0.2, 4), Indigo::Blue_Color);
	Indigo::Current_World.Add_Object(testies);
	table = Indigo::Current_World.Add_Object(Object(2.0, 0.5, -1.0, Mesh::Cube(1), Indigo::Red_Color));
	bounds = Indigo::Current_World.Add_Object(Object(0.0, 1.25, 0.0, Mesh::Box(10.0, 2.5, 20.0)));
	Indigo::Current_World.Add_Object(Object(0.0, 0.0, 10.0, Mesh::Load("C:\\Users\\Judah\\Documents\\GitHub\\CodeIndigo\\Release\\Monkey.obj")));
	Indigo::Current_World.Add_Object(Object(0.0, 0.0, -15.0, Mesh::Load("C:\\Users\\Judah\\Documents\\GitHub\\CodeIndigo\\Release\\Train.obj")));
	//Indigo::Current_World.Add_Object(Object(0.0, 0.0, 0.0, Mesh::Rectangle(10000.0, 10000.0), nullptr, 40.0, nullptr, false, Direction(1.0, 0.0, 90.0)));
	//Animation(&Indigo::Current_World.Get_Object(table), 100.0, 0.5, -1.0, 600);
	//Animation(&Indigo::Current_World.Get_Object(Indigo::Current_World.Add_Object(Object(0.0, -2.5, 0.0, Mesh::Cube(1.0)))), 0.0, -100.5, 0.0, 960);
	//srand(time(0));
	//for (int cube = 0; cube < 6000; ++cube)
	//{
	//	float * color = new float[3];
	//	color[0] = (rand() % 100) / (float) (rand() % 100);
	//	color[1] = (rand() % 100) / (float) (rand() % 100);
	//	color[2] = (rand() % 100) / (float) (rand() % 100);
	//	Object object = Object(rand() % 50 - 25.5, rand() % 20 - 10.5, rand() % 50 - 25.5, Mesh::Cube(1), color);
	//	//Animation(&object, rand() % 50 - 25.5, rand() % 20 - 10.5, rand() % 50 - 25.5, 100);
	//	Indigo::Current_World.Add_Object(object);
	//}
	Indigo::Update_Function = update;
	Indigo::Relative_Mouse_Moved_Function = mouse_moved;
	Indigo::Current_World.lighting.Set_Ambient(0.15);
	Direction light = Direction(1.0, 45.0, 45.0);
	Indigo::Current_World.lighting.Add_Light(light.Get_X(), light.Get_Y(), light.Get_Z(), true);
	Indigo::Current_World.lighting.Add_Light(0, 0, 0);
	Indigo::Current_World.camera.Place(0.0, 0.0, 0.0);
	Indigo::Run();
	return 0;
}
