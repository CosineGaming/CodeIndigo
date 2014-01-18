// Entry point!
// Code indigo is a 3d mystery game. Readme for more information.

#include "CodeIndigo.h"
#include <iostream>
#include "time.h"

int bounds;
int table;

Object player = Object(0, 0.75, 0);

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

void update(int time)
{

	static int frames = -1;
	frames++;

	if (frames == 20)
	{
		std::cout << "Beginning to load.\n";
		World world;
		std::cout << "Loading high-resolution sphere.\n";
		Object testies = Object(10.0, 2.0, 5.0, Mesh::Sphere(0.8, 2), Indigo::Blue_Color);
		std::cout << "Adding sphere to world.\n";
		world.Add_Object(testies);
		std::cout << "Loading simple cube as table.\n";
		table = world.Add_Object(Object(2.0, 0.5, -1.0, Mesh::Cube(1), Indigo::Red_Color));
		std::cout << "Table added to world. Loading containment room.\n";
		bounds = world.Add_Object(Object(0.0, 1.25, 0.0, Mesh::Box(10.0, 2.5, 20.0)));
		std::cout << "Room added. Loading light source marker.\n";
		world.Add_Object(Object(0.0, 1.0, 0.0, Mesh::Sphere(0.2, 3), Indigo::Green_Color));
		std::cout << "Loading model of Monkey.\n";
		world.Add_Object(Object(0.0, -10.0, 0.0, Mesh::Load("C:\\Users\\Judah\\Documents\\GitHub\\CodeIndigo\\Release\\Monkey.obj")));
		std::cout << "Loading model of flying train.\n";
		world.Add_Object(Object(0.0, 0.0, -25.0, Mesh::Load("C:\\Users\\Judah\\Documents\\GitHub\\CodeIndigo\\Release\\Train.obj")));
		std::cout << "Placing camera.\n";
		world.camera.Place(0.0, 0.0, 0.0);
		world.camera.X = world.Get_Object(4).X;
		world.camera.Y = world.Get_Object(4).Y + 1.5;
		world.camera.Z = world.Get_Object(4).Z;
		std::cout << "Adding ground.\n";
		Object ground = Object(0.0, 0.0, 0.0, Mesh::Rectangle(10000.0, 10000.0), nullptr, 40.0, nullptr, false, Direction(1.0, 0.0, 90.0));
		ground.Set_Hitbox(5000, 0, 5000, -5000, 0, -5000);
		world.Add_Object(ground);
		//Animation(&world.Get_Object(table), 100.0, 0.5, -1.0, 600);
		//Animation(&world.Get_Object(world.Add_Object(Object(0.0, -2.5, 0.0, Mesh::Cube(1.0)))), 0.0, -100.5, 0.0, 960);
		std::cout << "Generating large Cube structure.\n";
		srand(123);
		Object cube_object(0.0, 0.0, 0.0, Mesh::Cube(0.8));
		for (int cube = 0; cube < 1000; ++cube)
		{
			float * color = new float[3];
			color[0] = (rand() % 100) / (float) (rand() % 100);
			color[1] = (rand() % 100) / (float) (rand() % 100);
			color[2] = (rand() % 100) / (float) (rand() % 100);
			cube_object.Place(rand() % 100, rand() % 50 + 50, rand() % 100);
			cube_object.object_color = color;
			//Animation(&object, rand() % 50 - 25.5, rand() % 20 - 10.5, rand() % 50 - 25.5, 100);
			world.Add_Object(cube_object);
			if (cube % 200 == 0)
				std::cout << cube * 100 / 999 << "% loaded...\n";
		}
		std::cout << "Generating small graph...\n";
		Mesh graph = Mesh(4);
		for (float x = -1; x < 1; x+=0.02)
		{
			for (float z = -1; z < 1; z+=0.02)
			{
				for (float add = 0; add < 0.04; add+=0.02)
				{
					graph += Vertex(x + add, (x + add) * z, z);
					graph += Vertex(x + add, (x + add) * z, z);
				}
			}
		}
		world.Add_Object(Object(10.0, 2.0, 10.0, graph, Indigo::Red_Color));
		std::cout << "Switching worlds.\n";
		Indigo::Current_World = world;
		std::cout << "Initializing lighting state.\n";
		world.lighting.Set_Ambient(0.15);
		Direction light = Direction(1.0, 45.0, 45.0);
		world.lighting.Add_Light(light.Get_X(), light.Get_Y(), light.Get_Z(), true);
		world.lighting.Add_Light(0, 1.0, 0);
		std::cout << "Loaded.\n";
		Indigo::keys['r'] = true;
	}

	if (frames > 20)

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
			player.Place(10, 0, 10);
			Indigo::keys['r'] = false;
		}

		if (Indigo::keys[' '] && Indigo::Current_World.Collide(player) != -1)
		{
			gravity = 2.8;
		}

		if (Indigo::Current_World.Collide(player) == -1 || gravity > 0)
		{
			if (gravity > -50)
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

		if (player.Y - Indigo::Current_World.camera.Y < 0.5)
		{
			Indigo::Current_World.camera.Place(player.X, player.Y + 0.75, player.Z);
		}
		Indigo::Current_World.camera.eye = player.facing;

		static int direction = 1;
		Indigo::Current_World.Get_Object(4).facing.Add_Direction(0, .36 * time * direction);
		//if (rand() % (1000 / (time > 0 && time < 1000 ? time : 1)) == 0)	direction *= -1;
		//Indigo::Current_World.Get_Object(4).Move(.0002 * (rand() % 20 + 1) * time);

	}

}

int main(int argc, char ** argv)
{
	std::cout << "Initializing rendering environment.\n";
	float color[3] = {0.0, 0.0, 0.0};
	Indigo::Initialize(argc, argv, "Code Indigo", 48, true);//, color);
	std::cout << "Setting up loading world.\n";
	Indigo::Current_World.Add_Object(Object(0, 0, -1, Mesh::Load("Loading.obj")));
	std::cout << "Setting up callbacks.\n";
	Indigo::Update_Function = update;
	Indigo::Relative_Mouse_Moved_Function = mouse_moved;
	std::cout << "Showing GUI for loading.\n";
	Indigo::Run();
	return 0;
}
