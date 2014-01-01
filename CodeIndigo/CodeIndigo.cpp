// Entry point!
// Code indigo is a 3d mystery game. Readme for more information.

#include "CodeIndigo.h"
#include <iostream>
#include "time.h"

int bounds;
int table;

Object player = Object(0, 0.75, 0);

void update(int time)
{

	float speed = .0015 * time;

	static float gravity = 0;

	player.Set_Hitbox(0.25, 0.75, 0.1, -0.25, -0.75, -0.1);

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
		player.Move(speed);
	}
	if (Indigo::keys['s'])
	{
		player.Move(-speed);
	}
	if (Indigo::keys['a'])
	{
		player.Move(0.0, speed);
	}
	if (Indigo::keys['d'])
	{
		player.Move(0.0, -speed);
	}
	if (Indigo::keys['r'])
	{
		player.Place(0, 0.75, 0);
	}

	if (Indigo::keys[' '] && Indigo::Current_World.Collide(player) != -1)
	{
		gravity = 2.8;
	}

	if (Indigo::Current_World.Collide(player) == -1 || gravity > 0)
	{
		gravity -= .00980665 * time;
		if (!Indigo::keys['q'] && !Indigo::keys['f'])
			player.Move(0.0, 0.0, gravity * time / 1000);
		if (Indigo::keys['f'])
			player.Move(0.0, 0.0, gravity * time / -1000);
	}
	else
	{
		gravity = 0;
		player.Y = Indigo::Current_World.Get_Object(Indigo::Current_World.Collide(player)).Y + Indigo::Current_World.Get_Object(Indigo::Current_World.Collide(player)).Data.Hitbox[1].Y + 0.75;
	}

	if (Indigo::keys['3'])
	{
		std::cout << "Position: " << player.X << ", " << player.Y << ", " << player.Z << std::endl
			<< 1000 / time << " FPS" << std::endl << std::endl;
	}
	if (GL_NO_ERROR != glGetError())
	{
		std::cout << "\aError: " << glGetError() << std::endl;
	}
	if (Indigo::keys['`'])
	{
		exit(0);
	}

	Indigo::Current_World.camera.Place(player.X, player.Y + 0.75, player.Z);
	Indigo::Current_World.camera.eye = player.facing;

	static int direction = 1;
	Indigo::Current_World.Get_Object(4).facing.Add_Direction(0, .36 * time * direction);
	if (rand() % (1000 / (time > 0 && time < 1000 ? time : 1)) == 0)	direction *= -1;
	Indigo::Current_World.Get_Object(4).Move(.0002 * (rand() % 20 + 1) * time);

}

void mouse_moved(int x, int y)
{

	static const float sensitivity = 0.2;

	float y_angle = player.facing.Get_Y_Angle() + y * -1 * sensitivity;

	if (!(y_angle > 89 && y_angle <= 271))
	{
		player.facing.Add_Direction(0.0, x * -1 * sensitivity,
			y * -1 * sensitivity);
	}
	else
	{
		if (y_angle < 180)
		{
			player.facing.Set_Direction(1.0, player.facing.Get_X_Angle() + x * -1 * sensitivity, 89);
		}
		else
		{
			player.facing.Set_Direction(1.0, player.facing.Get_X_Angle() + x * -1 * sensitivity, 271);
		}
	}

}

int main(int argc, char ** argv)
{
	Direction asdf(1.0, 0.0, 90.0);
	std::cout << asdf.Get_X_Angle() << ", " << asdf.Get_Y_Angle() << std::endl;
	std::cout << "Initializing rendering environment.\n";
	float color[3] = {0.0, 0.0, 0.0};
	Indigo::Initialize(argc, argv, "Code Indigo", 48, true);//, color);
	std::cout << "Setting up callbacks.\n";
	Indigo::Update_Function = update;
	Indigo::Relative_Mouse_Moved_Function = mouse_moved;
	std::cout << "Initializing lighting state.\n";
	Indigo::Current_World.lighting.Set_Ambient(0.15);
	Direction light = Direction(1.0, 45.0, 45.0);
	Indigo::Current_World.lighting.Add_Light(light.Get_X(), light.Get_Y(), light.Get_Z(), true);
	Indigo::Current_World.lighting.Add_Light(0, 1.0, 0);
	std::cout << "Loading high-resolution sphere.\n";
	Object testies = Object(10.0, 2.0, 5.0, Mesh::Sphere(0.8, 5), Indigo::Blue_Color);
	std::cout << "Adding sphere to world.\n";
	Indigo::Current_World.Add_Object(testies);
	std::cout << "Loading simple cube as table.\n";
	table = Indigo::Current_World.Add_Object(Object(2.0, 0.5, -1.0, Mesh::Cube(1), Indigo::Red_Color));
	std::cout << "Table added to world. Loading containment room.\n";
	bounds = Indigo::Current_World.Add_Object(Object(0.0, 1.25, 0.0, Mesh::Box(10.0, 2.5, 20.0)));
	std::cout << "Room added. Loading light source marker.\n";
	Indigo::Current_World.Add_Object(Object(0.0, 1.0, 0.0, Mesh::Sphere(0.2, 3), Indigo::Green_Color));
	std::cout << "Loading model of Monkey.\n";
	Indigo::Current_World.Add_Object(Object(10.0, -10.0, 10.0, Mesh::Load("C:\\Users\\Judah\\Documents\\GitHub\\CodeIndigo\\Release\\Monkey.obj")));
	std::cout << "Loading model of flying train.\n";
	Indigo::Current_World.Add_Object(Object(0.0, 0.0, -25.0, Mesh::Load("C:\\Users\\Judah\\Documents\\GitHub\\CodeIndigo\\Release\\Train.obj")));
	std::cout << "Placing camera.\n";
	Indigo::Current_World.camera.Place(0.0, 0.0, 0.0);
	Indigo::Current_World.camera.X = Indigo::Current_World.Get_Object(4).X;
	Indigo::Current_World.camera.Y = Indigo::Current_World.Get_Object(4).Y + 1.5;
	Indigo::Current_World.camera.Z = Indigo::Current_World.Get_Object(4).Z;
	std::cout << "Adding ground.\n";
	Indigo::Current_World.Add_Object(Object(0.0, 0.0, 0.0, Mesh::Rectangle(10000.0, 10000.0), nullptr, 40.0, nullptr, false, Direction(1.0, 0.0, 90.0)));
	Animation(&Indigo::Current_World.Get_Object(table), 100.0, 0.5, -1.0, 600);
	Animation(&Indigo::Current_World.Get_Object(Indigo::Current_World.Add_Object(Object(0.0, -2.5, 0.0, Mesh::Cube(1.0)))), 0.0, -100.5, 0.0, 960);
	srand(time(0));
	Mesh cube_mesh = Mesh::Cube(1);
	for (int cube = 0; cube < 6000; ++cube)
	{
		float * color = new float[3];
		color[0] = (rand() % 100) / (float) (rand() % 100);
		color[1] = (rand() % 100) / (float) (rand() % 100);
		color[2] = (rand() % 100) / (float) (rand() % 100);
		Object object = Object(rand() % 50 - 25.5, rand() % 20 + 10.5, rand() % 50 - 25.5, cube_mesh, color);
		//Animation(&object, rand() % 50 - 25.5, rand() % 20 - 10.5, rand() % 50 - 25.5, 100);
		Indigo::Current_World.Add_Object(object);
	}
	std::cout << "Completely loaded. Ready to run. Preparing first frame.\n";
	Indigo::Run();
	return 0;
}
