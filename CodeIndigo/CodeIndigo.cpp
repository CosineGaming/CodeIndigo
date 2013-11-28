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
	if (Indigo::keys['q'])
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

	if (Indigo::keys [' '] && Indigo::Current_World.Collide(Vertex(camera.X, camera.Y - 1.5, camera.Z)))
	{
		gravity = 0.01421 * time;
	}
	if (!Indigo::Current_World.Collide(Vertex(camera.X, camera.Y - 1.5, camera.Z)) || gravity > 0)
	{
		gravity -= .00980665 * time;
		camera.Move(0.0, 0.0, gravity);
	}
	else
	{
		gravity = 0;
		camera.Y = int (camera.Y - 1.5) - (camera.Y > 0) + 1.5;
	}

	if (Indigo::keys ['3'])
	{
		std::cout << "Position: " << camera.X << ", " << camera.Y << ", " << camera.Z << std::endl
			<< 1000 / time << " FPS" << std::endl << std::endl;
	}


	if (Indigo::keys ['`'])
	{
		exit(0);
	}

	if (GL_NO_ERROR != glGetError())
	{
		std::cout << "\aError: " << glGetError() << std::endl;
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
	Direction test = Direction::Coordinates(20.0, 90.0, 0.0);
	std::cout << test.Get_X() << ", " << test.Get_Y() << ", " << test.Get_Z() << std::endl;
	std::cout << test.Dot(Direction::Coordinates(2.0, 1.5, 10.0));
	Indigo::Initialize(argc, argv, "Code Indigo");
	Indigo::Current_World.Add_Object(Object(-1.0, 1.7, 0.0, Mesh::Sphere(0.4, 4), Indigo::Blue_Color));
	table = Indigo::Current_World.Add_Object(Object(2.0, 0.5, -1.0, Mesh::Cube(1), Indigo::Red_Color));
	bounds = Indigo::Current_World.Add_Object(Object(0.0, 1.25, 0.0, Mesh::Box(10.0, 2.5, 5.0)));
	Indigo::Current_World.Add_Object(Object(0.0, -2.5, 0.0, Mesh::Cube(1.0)));
	srand(time(0));
	for (int cube=0; cube<2000; ++cube)
	{
		float * color = new float[3];
		color[0] = (rand() % 100) / (float) (rand() % 100);
		color[1] = (rand() % 100) / (float) (rand() % 100);
		color[2] = (rand() % 100) / (float) (rand() % 100);
		Indigo::Current_World.Add_Object(Object(rand() % 50 - 25.5, rand() % 20 - 10.5, rand() % 50 - 25.5, Mesh::Cube(1), color));
	}
	Indigo::Update_Function = update;
	Indigo::Relative_Mouse_Moved_Function = mouse_moved;
	Indigo::Current_World.lighting.Add_Light(0.0, 1.0, 10.0);
	Indigo::Current_World.camera.Place(0.0, 0.0, 0.0);
	Indigo::Run();
	return 0;
}
