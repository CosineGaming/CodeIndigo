#include "Indigo\IndigoEngine.h"
#include <iostream>
#include <ctime>

Object player;

float Color_Values[15] = {
	0.8, 0.0, 0.0,
	1.0, 0.6, 0.0,
	0.0, 0.7, 0.7,
	0.0, 1.0, 0.0,
	0.0, 0.1, 0.6 };

bool Render_Colors[5] = { false, false, false, false, false };

void check_render(int time, Object& self)
{
	self.Y = Render_Colors[(self.object_color - Color_Values) / 3] ? 4 : -4;
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
				Object add(x * 8 - 40, 2, z * 8 - 100, Mesh::Cube(7.9), &Color_Values[rand() % 5 * 3]);
				add.Update = check_render;
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

		static float speed = .001 * time;

		player.Move(speed);

		if (Indigo::keys['e'])
		{
			for (int i = 0; i < 5; ++i)
			{
				Render_Colors[i] = false;
			}
		}
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

		Camera& camera = Indigo::Current_World.camera;
		if (Indigo::Current_World.Collide(player) == -1)
		{
			camera.Place(player.X, player.Y + 0.75, player.Z);
		}
		else
		{
			if (Indigo::Current_World.Collide(player, camera.X - player.X) == -1)
			{
				camera.Z = player.Z;
			}
			if (Indigo::Current_World.Collide(player, 0, 0, camera.Z - player.Z) == -1)
			{
				camera.X = player.X;
			}
			player.Place(camera.X, camera.Y - 0.75, camera.Z);
		}
		static int last = -1;
		int now = (Indigo::Current_World.Get_Object(Indigo::Current_World.Collide(player, 0, -1)).object_color - Color_Values) / 3;
		if (last != now && last != -1)
		{
			Render_Colors[last] = true;
			if (Render_Colors[0] + Render_Colors[1] + Render_Colors[2] + Render_Colors[3] + Render_Colors[4] == 4)
			{
				for (int i = 0; i < 5; ++i)
				{
					Render_Colors[i] = false;
				}
			}
		}
		last = now;
		camera.eye = player.facing;

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
	Indigo::FPS_Mouse(&player);
	std::cout << "Showing GUI for loading.\n";
	Indigo::Run();
}