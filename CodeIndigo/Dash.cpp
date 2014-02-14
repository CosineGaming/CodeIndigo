#include "Indigo\IndigoEngine.h"
#include <iostream>
#include <ctime>

Object * player = new Object(0.0, 0.75, 0.0);

const int Number_Of_Colors = 4;

const int Cube_Size = 10;

const int Platform_Size = 21;

float Health = 100;

float Pause_Time = 100;

void load(int time);

float Color_Values[15] = {
	0.8, 0.0, 0.0,
	0.0, 0.7, 0.7,
	0.0, 1.0, 0.0,
	0.3, 0.0, 0.3,
	0.0, 0.0, 0.0
};

bool Render_Colors[5] = { false, false, false, false, false };

void generate_colors(void)
{
	for (int i = 0; i < 15; ++i)
	{
		Color_Values[i] = (rand() % 256) / 255.0;
		for (int c = 0; c < i; c+=3)
		{
			while (abs(Color_Values[c] - Color_Values[i]) < 0.2)
			{
				Color_Values[i] = (rand() % 256) / 255.0;
			}
		}
	}
}

void check_render(int time, Object& self)
{
	self.Y = Render_Colors[(self.object_color - Color_Values) / 3] && !self.Collide(*player, 0, -1) ? Cube_Size / 2.0 : Cube_Size / -2.0;
}

void show(int time, Object& self)
{
	if (Render_Colors[(self.object_color - Color_Values) / 3])
	{
		self.Is_Blank = true;
	}
	else
	{
		self.Is_Blank = false;
		Direction distance = player->facing;
		distance.Normalize();
		distance.Add_Direction(-0.3, (self.object_color - Color_Values) / 3 * (30.0 / (Number_Of_Colors - 1)) - 15, -10);
		self.X = player->X + distance.Get_X();
		self.Y = player->Y + distance.Get_Y() + 0.75;
		self.Z = player->Z + distance.Get_Z();
	}
}

void check_health(int time, Object& self)
{
	float health_pos = Health * 0.75 / 100;
	self.Data.vertices[0].X = -1 * health_pos;
	self.Data.vertices[1].X = health_pos;
	self.Data.vertices[2].X = health_pos;
	self.Data.vertices[3].X = -1 * health_pos;
}

void pause(int time, Object& self)
{
	static int label = -1;
	if (Pause_Time == 100)
	{
		self.Is_Blank = true;
		if (label != -1)
		{
			Indigo::Current_World.Remove_Text(label);
			label = -1;
		}
	}
	else
	{
		if (label == -1)
		{
			static const Text text = Text(-0.1, -0.81, "Pause Time", Indigo::Black_Color, GLUT_BITMAP_9_BY_15);
			label = Indigo::Current_World.Add_Text(text);
		}
		self.Is_Blank = false;
		float pause_pos = Pause_Time * 1 / 100;
		self.Data.vertices[0].X = -1 * pause_pos;
		self.Data.vertices[1].X = pause_pos;
		self.Data.vertices[2].X = pause_pos;
		self.Data.vertices[3].X = -1 * pause_pos;
	}
}

void tutorial(void)
{
	static int index = 0;
	char * set [] = { "Welcome to dash. Press 'q' at any time to unpause the game.", "You are in the center of a platform. Your goal is to get off of it.",
		"But these blocks aren't stationary. Step on a red tile, and all red tiles will block your way.", "The only way to step on that color of tile again is to step on ALL colors of tile.",
		"You will be constantly racing forward. No way to control that.", "A few cubes will appear in front of you. Those tiles remain to be stepped on.",
		"You might get stuck with tiles all around you. Press 'e' to restart.", "Press 'q' to pause / unpause the game.",
		"Move the mouse to look around. Press space to see from the top.", "I think you'll get the hang of it. Press 'q' to begin. Good luck!" };
	if (index < 10)
	{
		Indigo::Current_World.Add_Text(Text(-0.35, -0.5, set[index], nullptr, GLUT_BITMAP_9_BY_15, 420, tutorial));
		++index;
	}
}

void update(int time, Object& self)
{

	static bool running = false;

	static float speed = .015 * time;

	static Vertex old = Vertex(0, 0, 0);

	static int last = -1;

	Camera& camera = Indigo::Current_World.camera;

	if (Indigo::keys['q'])
	{
		running = !running;
		Indigo::keys['q'] = false;
	}

	if (Indigo::keys['e'])
	{
		for (int i = 0; i < Number_Of_Colors; ++i)
		{
			Render_Colors[i] = false;
		}
		self.Place(0.0, 0.75, 0.0);
		self.facing = Direction();
		old = Vertex(0.0, 0.75, 0.0);
		last = -1;
		camera.Place(0.0, 1.5, 0.0);
		Health = 100;
		Pause_Time = 100;
	}

	if (Indigo::keys['r'])
	{
		Indigo::Current_World = World();
		Mesh loading = Mesh::Rectangle(1.0, 1.0);
		loading.Texture("Textures\\Loading.bmp");
		Indigo::Current_World.Add_2D_Object(Object(0, 0, 1, loading));
		Indigo::Update_Function = load;
		return;
	}

	if (Indigo::keys['c'])
	{
		generate_colors();
		Indigo::keys['c'] = false;
	}

	if (Indigo::keys[','])
	{
		self.Y = 1.5 * Cube_Size;
	}

	static bool verbose = false;
	if (Indigo::keys['3'])
	{
		verbose = !verbose;
		Indigo::keys['3'] = false;
	}
	if (verbose)
	{
		static char * fps = new char[3];
		_itoa_s(1000 / time, fps, 3, 10);
		std::cout << fps[0] << fps[1] << fps[2] << std::endl;
		Indigo::Current_World.Add_Text(Text(-1.0, 0.8, fps, nullptr, GLUT_BITMAP_9_BY_15));
		std::cout << "Position: " << self.X << ", " << self.Y << ", " << self.Z << std::endl
			<< 1000 / time << " FPS" << std::endl << std::endl;
	}
	if (GL_NO_ERROR != glGetError())
	{
		Indigo::Current_World.Add_Text(Text(-0.5, 0.0, "Error. Please check Console window.", nullptr, GLUT_BITMAP_9_BY_15));
		std::cout << "\aError: " << glGetError() << std::endl;
	}
	if (Indigo::keys['`'])
	{
		exit(0);
	}

	if (running)
	{
		if (Pause_Time < 100)
		{
			Pause_Time += 0.01 * time;
		}
		else
		{
			Pause_Time = 100;
		}
		if (Health > 0)
		{
			self.Move(speed);
			if (Health < 100)
			{
				Health += 0.0075 * time;
			}
			else
			{
				Health = 100;
			}
		}
		else
		{
			Health = 100;
			for (int i = 0; i < Number_Of_Colors; ++i)
			{
				Render_Colors[i] = false;
			}
			self.Place(0.0, 0.75, 0.0);
			self.facing = Direction();
			old = Vertex(0.0, 0.75, 0.0);
			camera.Place(0.0, 1.5, 0.0);
			running = false;
			Pause_Time = 100;
		}
	}
	else
	{
		if (Pause_Time > 0)
		{
			Pause_Time -= .05 * time;
		}
		else
		{
			Pause_Time = 0;
			running = true;
		}
	}

	if (Indigo::Current_World.Collide(self) == -1)
	{
		camera.Place(self.X, self.Y + 0.75, self.Z);
	}
	else
	{
		if (running)
		{
			Health -= 0.2575 * time;
		}
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

	int now = (Indigo::Current_World.Get_Object(Indigo::Current_World.Collide(self, 0, -1)).object_color - Color_Values) / 3;
	if (last != now && now >= 0 && now <= Number_Of_Colors - 1)
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

	old = Vertex(self.X, self.Y, self.Z);

	player = &self;

}

void load(int time)
{

	static int wait = 15;
	if (wait)
	{
		--wait;
		return;
	}
	std::cout << "Beginning to load.\n";
	World world;
	std::cout << "Initializing walls\n";
	srand(std::time(0));
	for (int x = 0; x < Platform_Size; ++x)
	{
		for (int z = 0; z < Platform_Size; ++z)
		{
			if (!(x == Platform_Size / 2 && z == Platform_Size / 2))
			{
				world.Add_Object(Object((x - Platform_Size / 2.0 + 0.5) * Cube_Size, Cube_Size / -2.0, (z - Platform_Size / 2.0 + 0.5) * Cube_Size, Mesh::Cube(Cube_Size), &Color_Values[rand() % Number_Of_Colors * 3], 60, check_render));
			}
			else
			{
				world.Add_Object(Object(0.0, Cube_Size / -2.0, 0.0, Mesh::Cube(Cube_Size), nullptr, 60));
			}
		}
	}
	world.Add_Object(Object((Platform_Size / 2.0 + 0.5) * Cube_Size, Cube_Size * 1.5, 0, Mesh::Box(Cube_Size, Cube_Size, (Platform_Size + 2) * Cube_Size)));
	world.Add_Object(Object((Platform_Size / -2.0 - 0.5) * Cube_Size, Cube_Size * 1.5, 0, Mesh::Box(Cube_Size, Cube_Size, (Platform_Size + 2) * Cube_Size)));
	world.Add_Object(Object(0, Cube_Size * 1.5, (Platform_Size / 2.0 + 0.5) * Cube_Size, Mesh::Box(Platform_Size * Cube_Size, Cube_Size, Cube_Size)));
	world.Add_Object(Object(0, Cube_Size * 1.5, (Platform_Size / -2.0 - 0.5) * Cube_Size, Mesh::Box(Platform_Size * Cube_Size, Cube_Size, Cube_Size)));
	std::cout << "Setting player up.\n";
	int added = world.Add_Object(Object(0.0, 0.75, 0.0, Mesh::Load("Meshes\\Arrow.obj"), nullptr, 60, update));
	Object& point = world.Get_Object(added);
	point.Set_Hitbox();
	world.camera.Place(point.X, 1.5, point.Z);
	std::cout << "Setting up HUD.\n";
	for (int i = 0; i < Number_Of_Colors; ++i)
	{
		world.Add_Object(Object(0, 10, 0, Mesh::Cube(0.075), &Color_Values[i * 3], 60, show));
	}
	world.Add_2D_Object(Object(0.0, -0.65, 0.0, Mesh::Rectangle(1.5, 0.075), Indigo::Red_Color, 60, check_health));
	world.Add_Text(Text(-0.07, -0.66, "Health", Indigo::White_Color, GLUT_BITMAP_9_BY_15));
	world.Add_2D_Object(Object(0.0, -0.8, 0.0, Mesh::Rectangle(1.0, 0.025), nullptr, 60, pause));
	std::cout << "Changing worlds.\n";
	Indigo::Current_World = world;
	std::cout << "Initializing lighting state.\n";
	Indigo::Current_World.lighting.Set_Ambient(0.15);
	Direction light = Direction(1.0, 45.0, 45.0);
	Indigo::Current_World.lighting.Add_Light(light.Get_X(), light.Get_Y(), light.Get_Z(), true);
	std::cout << "Performing world-specific tasks.\n";
	delete player;
	player = &Indigo::Current_World.Get_Object(added);
	Indigo::FPS_Mouse(player);
	//tutorial();
	std::cout << "Removing loading routine from queue.\n";
	Indigo::Update_Function = nullptr;
	std::cout << "Loaded. Next frame will run new world.\n";

}

int main(int argc, char ** argv)
{
	std::cout << "Initializing rendering environment.\n";
	float color[3] = { 0.0, 0.0, 0.0 };
	Indigo::Initialize(argc, argv, "Code Indigo", 60, true, color);
	std::cout << "Setting up loading world.\n";
	Mesh loading = Mesh::Rectangle(2.0, 2.0);
	loading.Texture("Textures\\Loading.bmp");
	Indigo::Current_World.Add_2D_Object(Object(0, 0, 0, loading));
	std::cout << "Setting up callbacks.\n";
	Indigo::Update_Function = load;
	std::cout << "Showing GUI for loading.\n";
	Indigo::Run();
	return 0;
}