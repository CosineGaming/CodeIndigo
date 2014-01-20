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
		Mesh sphere = Mesh::Sphere(2.4, 1);
		sphere.Texture("C:\\Users\\Judah\\Documents\\GitHub\\CodeIndigo\\Release\\Rough_Cloth.bmp");
		Object testies = Object(10.0, 2.0, 5.0, sphere, Indigo::Blue_Color);
		world.Add_Object(testies);
		std::cout << "Loading simple cube as table.\n";
		table = world.Add_Object(Object(2.0, 0.5, -1.0, Mesh::Cube(1), Indigo::Red_Color));
		std::cout << "Loading containment room.\n";
		Mesh room = Mesh::Box(10.0, 2.5, 20.0);
		room.Texture("C:\\Users\\Judah\\Documents\\GitHub\\CodeIndigo\\Release\\Dark_Wood.bmp");
		bounds = world.Add_Object(Object(0.0, 1.25, 0.0, room));
		std::cout << "Loading light source marker.\n";
		world.Add_Object(Object(0.0, 1.0, 0.0, Mesh::Sphere(0.2, 3), Indigo::Green_Color));
		std::cout << "Loading model of Monkey.\n";
		world.Add_Object(Object(0.0, -10.0, 0.0, Mesh::Load("C:\\Users\\Judah\\Documents\\GitHub\\CodeIndigo\\Release\\Monkey.obj")));
		std::cout << "Loading model of flying train.\n";
		Mesh train = Mesh::Load("C:\\Users\\Judah\\Documents\\GitHub\\CodeIndigo\\Release\\Train.obj");
		train.Texture("C:\\Users\\Judah\\Documents\\GitHub\\CodeIndigo\\Release\\Screen.bmp");
		world.Add_Object(Object(0.0, 0.0, -25.0, train));
		std::cout << "Adding texture test.\n";
		Mesh tex = Mesh(4);
		tex += Vertex(0.0, 0.0, 0.0);
		tex += Vertex(0.0, 0.0, 1.0);
		tex += Vertex(1.0, 0.0, 1.0);
		tex += Vertex(1.0, 0.0, 0.0);
		tex.Texture("C:\\Users\\Judah\\Documents\\GitHub\\CodeIndigo\\Release\\Wall.bmp");
		world.Add_Object(Object(-10.0, 2.0, -10.0, tex));
		std::cout << "Setting player up.\n";
		player.Place(0.0, 10.0, 0.0);
		Indigo::Current_World.camera.Place(player.X, player.Y + 0.75, player.Z);
		player.Set_Hitbox(0.25, 0.75, 0.1, -0.25, -0.75, -0.1);
		std::cout << "Adding ground.\n";
		Mesh floor = Mesh::Rectangle(10000.0, 10000.0);
		floor.Texture("C:\\Users\\Judah\\Documents\\GitHub\\CodeIndigo\\Release\\Floor.bmp");
		floor.Set_Texture_Coordinate(0, Vertex(0, 0));
		floor.Set_Texture_Coordinate(1, Vertex(20000, 0));
		floor.Set_Texture_Coordinate(2, Vertex(0, 20000));
		floor.Set_Texture_Coordinate(3, Vertex(20000, 20000));
		Object ground = Object(0.0, 0.0, 0.0, floor, nullptr, 40.0, nullptr, false, Direction(1.0, 0.0, 90.0));
		ground.Set_Hitbox(5000, 0, 5000, -5000, 0, -5000);
		world.Add_Object(ground);
		//Animation(&world.Get_Object(table), 100.0, 0.5, -1.0, 600);
		//Animation(&world.Get_Object(world.Add_Object(Object(0.0, -2.5, 0.0, Mesh::Cube(1.0)))), 0.0, -100.5, 0.0, 960);
		std::cout << "Generating large Cube structure.\n";
		srand(123);
		Mesh cube = Mesh::Cube(0.8);
		cube.Texture("C:\\Users\\Judah\\Documents\\GitHub\\CodeIndigo\\Release\\Couch.bmp");
		Object cube_object(0.0, 0.0, 0.0, cube);
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
		Indigo::Current_World.Add_Object(Object(0, 0, -1, Mesh::Load("Loading.obj")));
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
	}

	if (frames > 20)

	{

		static float sprinting = 100;

		float speed = .0015 * time;

		static float gravity = 0;

		if (Indigo::Shift)
		{
			if (sprinting > 0)
				speed += .000045 * sprinting * time;
				sprinting -= .014 * time;
		}
		else
		{
			if (sprinting < 100)
				sprinting += .028 * time;
		}
		if (Indigo::keys['e'])
		{
			speed += .01 * time;
		}
		if (Indigo::keys['x'])
		{
			speed += .05 * time;
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
			player.Place(0, 10, 0);
			Indigo::Current_World.camera.Place(player.X, player.Y + 0.75, player.Z);
		}

		if (Indigo::keys['y'])
		{
			Indigo::Field_Of_View += 1;
			Indigo::Reshape();
			std::cout << "Field of view: " << Indigo::Field_Of_View << std::endl;
		}
		if (Indigo::keys['t'])
		{
			Indigo::Field_Of_View -= 1;
			Indigo::Reshape();
			std::cout << "Field of view: " << Indigo::Field_Of_View << std::endl;
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

		if (player.Y + 0.75 - Indigo::Current_World.camera.Y < 0.5 || Indigo::keys['f'])
		{
			Indigo::Current_World.camera.Place(player.X, player.Y + 0.75, player.Z);
		}
		else
		{
			player.Place(Indigo::Current_World.camera.X, Indigo::Current_World.camera.Y - 0.75, Indigo::Current_World.camera.Z);
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
	Indigo::Initialize(argc, argv, "Code Indigo", 60, true);//, color);
	std::cout << "Setting up loading world.\n";
	Indigo::Current_World.Add_Object(Object(0, 0, -1, Mesh::Load("Loading.obj")));
	std::cout << "Setting up callbacks.\n";
	Indigo::Update_Function = update;
	Indigo::Relative_Mouse_Moved_Function = mouse_moved;
	std::cout << "Showing GUI for loading.\n";
	Indigo::Run();
	return 0;
}
