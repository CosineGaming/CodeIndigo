#include "Indigo\IndigoEngine.h"
#include <iostream>
#include <ctime>

Object * player = new Object(0.0, 0.75, 0.0);

const int Number_Of_Colors = 4;

const int Cube_Size = 15;

float Color_Values[15] = {
	0.8, 0.0, 0.0,
	0.0, 0.7, 0.7,
	0.0, 1.0, 0.0,
	0.3, 0.0, 0.3,
	0.0, 0.0, 0.0
};

bool Render_Colors[5] = { false, false, false, false, false };

void check_render(int time, Object& self)
{
	self.Y = Render_Colors[(self.object_color - Color_Values) / 3] && !self.Collide(*player, 0, -1) ? Cube_Size / 2.0 : Cube_Size / -2.0;
}

void show(int time, Object& self)
{
	self.Is_Blank = Render_Colors[(self.object_color - Color_Values) / 3];
  Direction distance = player->facing;
  distance.Normalize();
  distance.Add_Direction(0.2, (self.object_color - Color_Values) / 3 * (20.0 / (Number_Of_Colors - 1)) - 10, -10);
  self.X = player->X + distance.Get_X();
  self.Y = player->Y + distance.Get_Y() + 0.75;
  self.Z = player->Z + distance.Get_Z();
}

void update(int time, Object& self)
{

	static float speed = .001 * time;

	self.Move(speed);

	Camera& camera = Indigo::Current_World.camera;

	if (Indigo::keys['e'])
	{
		for (int i = 0; i < Number_Of_Colors; ++i)
		{
			Render_Colors[i] = false;
		}
		self.Place(0.0, 0.75, 0.0);
		camera.Place(0.0, 1.5, 0.0);
	}
	if (Indigo::keys['3'])
	{
		std::cout << "Position: " << self.X << ", " << self.Y << ", " << self.Z << std::endl
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

	if (Indigo::Current_World.Collide(self) == -1)
	{
		camera.Place(self.X, self.Y + 0.75, self.Z);
	}
	else
	{
		camera.Y = self.Y + 0.75;
		if (Indigo::Current_World.Collide(self, camera.X - self.X) == -1)
		{
			camera.Z = self.Z;
		}
		if (Indigo::Current_World.Collide(self, 0, 0, camera.Z - self.Z) == -1)
		{
			camera.X = self.X;
		}
		self.Place(camera.X, camera.Y - 0.75, camera.Z);
	}
	camera.eye = self.facing;
	if (Indigo::keys[' '])
	{
		camera.Watch(self, Direction(10, 90, 110));
	}

	static int last = -1;
	int now = (Indigo::Current_World.Get_Object(Indigo::Current_World.Collide(self, 0, -1)).object_color - Color_Values) / 3;
	if (last != now && now >= 0 && now <= 3)
	{
		Render_Colors[now] = true;
		bool all = true;
		for (int i = 0; i < Number_Of_Colors; ++i)
		{
			if (!Render_Colors[i])
			{
				all = false;
				break;
			}
		}
		if (all)
		{
			for (int i = 0; i < Number_Of_Colors; ++i)
			{
				Render_Colors[i] = false;
			}
		}
	}
	last = now;

	player = &self;

}

void load(int time)
{

	std::cout << "Beginning to load.\n";
	World world;
	std::cout << "Initializing walls\n";
	srand(std::time(0));
	for (int x = 0; x < 50; ++x)
	{
		for (int z = 0; z < 50; ++z)
		{
			Object add(x * Cube_Size - 25 * Cube_Size, Cube_Size / -2.0, z * Cube_Size - 25 * Cube_Size, Mesh::Cube(Cube_Size), &Color_Values[rand() % Number_Of_Colors * 3]);
			add.Update = check_render;
			world.Add_Object(add);
		}
	}
	std::cout << "Setting up HUD.\n";
	for (int i = 0; i < Number_Of_Colors; ++i)
	{
		Object add(i, 1, -1, Mesh::Cube(0.1), &Color_Values[i * 3]);
		add.Update = show;
		world.Add_Object(add);
	}
	std::cout << "Setting player up.\n";
	int added = world.Add_Object(Object(0.0, 0.75, 0.0, Mesh::Load("C:\\Users\\Judah\\Documents\\GitHub\\CodeIndigo\\Meshes\\Arrow.obj"), nullptr, 60, update));
	Object& point = world.Get_Object(added);
	point.Set_Hitbox();
	world.camera.Place(point.X, 1.5, point.Z);
	std::cout << "Changing worlds.\n";
	Indigo::Current_World = world;
	std::cout << "Initializing lighting state.\n";
	Indigo::Current_World.lighting.Set_Ambient(0.15);
	Direction light = Direction(1.0, 45.0, 45.0);
	Indigo::Current_World.lighting.Add_Light(light.Get_X(), light.Get_Y(), light.Get_Z(), true);
	std::cout << "Updating player and removing loading routine from queue.\nLoaded. Next frame will run new world.\n";
	player = &Indigo::Current_World.Get_Object(added);
	Indigo::FPS_Mouse(player);
	Indigo::Update_Function = nullptr;

}

int main(int argc, char ** argv)
{
	std::cout << "Initializing rendering environment.\n";
	float color[3] = { 0.0, 0.0, 0.0 };
	Indigo::Initialize(argc, argv, "Code Indigo", 60, true, color);
	std::cout << "Setting up loading world.\n";
	Mesh loading = Mesh::Rectangle(1.0, 1.0);
	loading.Texture("C:\\Users\\Judah\\Documents\\GitHub\\CodeIndigo\\Textures\\Loading.bmp");
	Indigo::Current_World.Add_Object(Object(0, 0, 1, loading));
	std::cout << "Setting up callbacks.\n";
	Indigo::Update_Function = load;
	std::cout << "Showing GUI for loading.\n";
	Indigo::Run();
}