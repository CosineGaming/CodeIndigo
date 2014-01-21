#include "Indigo\IndigoEngine.h"
#include <iostream>
#include "Wall.h"
#include <ctime>

Object player;

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
		std::cout << "Initializing walls\n";
		srand(std::time(0));
		for (int x = 0; x < 20; ++x)
		{
			for (int z = 0; z < 50; ++z)
			{
				Wall add(1, x, z);
				world.Add_Object(add);
			}
		}
		std::cout << "Setting player up.\n";
		player.Place(0.0, 0.75, 0.0);
		world.camera.Place(player.X, 1.5, player.Z);
		player.Set_Hitbox(0.25, 0.75, 0.1, -0.25, -0.75, -0.1);
		std::cout << "Changing worlds.\n";
		Indigo::Current_World = world;
		std::cout << "Initializing lighting state.\n";
		world.lighting.Set_Ambient(0.15);
		Direction light = Direction(1.0, 45.0, 45.0);
		world.lighting.Add_Light(light.Get_X(), light.Get_Y(), light.Get_Z(), true);
		world.lighting.Add_Light(0, 1.0, 0);
		std::cout << "Loaded. Next frame will run new world.\n";
	}

	if (frames > 20)

	{

		static float speed = .0005 * time;

		player.Move(speed);

		if (Indigo::keys['r'])
		{
			player.Place(0.0, 0.75, 0.0);
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

		//if (Indigo::Current_World.Collide(player) != -1)
		//{
			Indigo::Current_World.camera.Place(player.X, player.Y + 0.75, player.Z);
		//}
		//else
		//{
		//	std::cout << "Colliding!\n";
		//	player.Place(Indigo::Current_World.camera.X, Indigo::Current_World.camera.Y - 0.75, Indigo::Current_World.camera.Z);
		//
		Indigo::Current_World.camera.eye = player.facing;
		int blarg = 12;

	}

}

int main(int argc, char ** argv)
{
	std::cout << "Initializing rendering environment.\n";
	float color[3] = { 0.0, 0.0, 0.0 };
	Indigo::Initialize(argc, argv, "Code Indigo", 60, true, color);
	std::cout << "Setting up loading world.\n";
	Mesh loading = Mesh::Rectangle(1.0, 1.0);
	loading.Texture("C:\\Users\\Judah\\Documents\\GitHub\\CodeIndigo\\Release\\Loading.bmp");
	Indigo::Current_World.Add_Object(Object(0, 0, 1, loading));
	std::cout << "Setting up callbacks.\n";
	Indigo::Update_Function = update;
	Indigo::Relative_Mouse_Moved_Function = mouse_moved;
	std::cout << "Showing GUI for loading.\n";
	Indigo::Run();
}