#include "Indigo\IndigoEngine.h"
#include <iostream>
#include <ctime>

Object * player = new Object(0.0, 0.75, 0.0);

const int Number_Of_Colors = 4;

const int Cube_Size = 15;

const int Platform_Size = 30;

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

void tutorial(void)
{
  static int index = 0;
  char * set [] = { "Welcome to dash. Press 'P' at any time to unpause the game.", "You are in the center of a platform. Your goal is to get off of it.",
    "But these blocks aren't stationary. Step on a red tile, and all red tiles will block your way.", "The only way to step on that color of tile again is to step on ALL colors of tile.",
    "You will be constantly racing forward. No way to control that.", "A few cubes will appear in front of you. Those tiles remain to be stepped on.",
    "You might get stuck with tiles all around you. Press 'E' to restart.", "Press 'P' to pause / unpause the game.",
    "Move the mouse to look around. Press space to see from the top.", "I think you'll get the hang of it. Press 'P' to begin. Good luck!" };
  if (index < 10)
  {
    Indigo::Current_World.Add_Text(Text(-0.35, -0.5, set[index], nullptr, GLUT_BITMAP_9_BY_15, 420, tutorial));
    ++index;
  }
}

void update(int time, Object& self)
{

  static bool running = false;

	static float speed = .001 * time;

	Camera& camera = Indigo::Current_World.camera;

  if (Indigo::keys['p'])
  {
    running = !running;
    Indigo::keys['p'] = false;
  }

	if (Indigo::keys['e'] || Indigo::keys['p'])
	{
		for (int i = 0; i < Number_Of_Colors; ++i)
		{
			Render_Colors[i] = false;
		}
		self.Place(0.0, 0.75, 0.0);
		camera.Place(0.0, 1.5, 0.0);
    camera.eye = Direction(1.0);
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

  if (!running)
  {
    return;
  }

  self.Move(speed);

  static Vertex old = Vertex(0, 0, 0);
	if (Indigo::Current_World.Collide(self) == -1)
	{
		camera.Place(self.X, self.Y + 0.75, self.Z);
	}
	else
  {
		if (Indigo::Current_World.Collide(self, old.X - self.X) == -1)
		{
			old.Z = self.Z;
		}
		if (Indigo::Current_World.Collide(self, 0, 0, old.Z - self.Z) == -1)
		{
			old.X = self.X;
    }
    self.Place(old.X, old.Y, old.Z);
    camera.Place(self.X, self.Y + 0.75, self.Z);
	}
	camera.eye = self.facing;
	if (Indigo::keys[' '])
	{
    camera.Y += 20;
    self.facing.Normalize(10);
    camera.X += self.facing.Get_X() * -1;
    camera.Z += self.facing.Get_Z() * -1;
	}

	static int last = -1;
	int now = (Indigo::Current_World.Get_Object(Indigo::Current_World.Collide(self, 0, -1)).object_color - Color_Values) / 3;
	if (last != now && now >= 0 && now <= 3)
	{
    glClearColor(Color_Values[now * 3], Color_Values[now * 3 + 1], Color_Values[now * 3 + 2], 1);
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

  old = Vertex(self.X, self.Y, self.Z);

	player = &self;

}

void load(int time)
{

	std::cout << "Beginning to load.\n";
	World world;
	std::cout << "Initializing walls\n";
	srand(std::time(0));
	for (int x = 0; x < Platform_Size; ++x)
	{
		for (int z = 0; z < Platform_Size; ++z)
		{
      Object add = Object((x - Platform_Size / 2.0) * Cube_Size, Cube_Size / -2.0, (z - Platform_Size / 2.0) * Cube_Size, Mesh::Cube(Cube_Size), &Color_Values[rand() % Number_Of_Colors * 3]);
			add.Update = check_render;
			world.Add_Object(add);
		}
	}
	std::cout << "Setting player up.\n";
	int added = world.Add_Object(Object(0.0, 0.75, 0.0, Mesh::Load("C:\\Users\\Judah\\Documents\\GitHub\\CodeIndigo\\Meshes\\Arrow.obj"), nullptr, 60, update));
	Object& point = world.Get_Object(added);
	point.Set_Hitbox();
	world.camera.Place(point.X, 1.5, point.Z);
	std::cout << "Setting up HUD and tutorial.\n";
	for (int i = 0; i < Number_Of_Colors; ++i)
	{
    Direction distance = Direction(1, 0, 0);
    distance.Add_Direction(0.2, i / 3 * (20.0 / (Number_Of_Colors - 1)) - 10, -10);
    Object add(0, -1, 0, Mesh::Cube(0.1), &Color_Values[i * 3]);
		add.Update = show;
		world.Add_Object(add);
  }
	std::cout << "Changing worlds.\n";
	Indigo::Current_World = world;
	std::cout << "Initializing lighting state.\n";
	Indigo::Current_World.lighting.Set_Ambient(0.15);
	Direction light = Direction(1.0, 45.0, 45.0);
	Indigo::Current_World.lighting.Add_Light(light.Get_X(), light.Get_Y(), light.Get_Z(), true);
  std::cout << "Updating player and removing loading routine from queue.\nAdding tutorial.\n";
  delete player;
	player = &Indigo::Current_World.Get_Object(added);
	Indigo::FPS_Mouse(player);
  tutorial();
	Indigo::Update_Function = nullptr;
  std::cout << "Loaded. Next frame will run new world.\n";

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
  return 0;
}