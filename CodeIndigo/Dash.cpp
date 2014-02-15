#include "Indigo\IndigoEngine.h"
#include <iostream>
#include <ctime>

Object * player = new Object(0.0, 0.75, 0.0);

const int Number_Of_Colors = 5;
const int Cube_Size = 10;
const int Platform_Size = 21;

float Health = 100;
float Pause_Time = 100;

int Help_Index = 0;

World world;

void load(int time);

float Color_Values[15] = {
	0.8, 0.0, 0.0,
	0.0, 0.7, 0.7,
	0.0, 1.0, 0.0,
	0.3, 0.0, 0.3,
	0.0, 0.9, 0.8
};

bool Render_Colors[5] = { false, false, false, false, false };

std::vector<Vertex> Path = std::vector<Vertex>();

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

void reset_renders(void)
{
	for (int i = 0; i < Number_Of_Colors; ++i)
	{
		Render_Colors[i] = false;
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

void initialize(void)
{

	Indigo::Current_World = World();

	Path = std::vector<Vertex>();
	player = new Object();
	Health = 100;
	Pause_Time = 99;

	std::cout << "Setting up loading world.\n";
	Mesh loading = Mesh::Rectangle(2, 2);
	loading.Texture("Textures\\Loading.bmp");
	Indigo::Current_World.Add_2D_Object(Object(0, 0, 0, loading));
	std::cout << "Setting up callbacks.\n";
	Indigo::Update_Function = load;

}

void show_path(int time, Object& self)
{
	self.X = Path[Help_Index].X;
	self.Z = Path[Help_Index].Z;
	if (self.Collide(*player, 0, 1) && Help_Index < Path.size())
	{
		Help_Index++;
	}
}

bool possible(float x, float z, World& world)
{
	bool found = false;
	for (int i = 0; i < Path.size(); ++i)
	{
		if (Path[i].X == x && Path[i].Z == z)
		{
			return false;
		}
	}
	Path.push_back(Vertex(x, 0, z));
	if ((abs(x) > Platform_Size * Cube_Size / 2.0 || abs(z) > Platform_Size * Cube_Size / 2.0))
	{
		return true;
	}
	Object& current = world.Get_Object(world.Collide(Vertex(x, Cube_Size / -2.0, z)));
	if (current.object_color)
	{
		if (Render_Colors[(current.object_color - Color_Values) / 3])
		{
			Path.pop_back();
			return false;
		}
		Render_Colors[(current.object_color - Color_Values) / 3] = true;
		bool all = true;
		for (int i = 0; i < Number_Of_Colors; ++i)
		{
			if (!Render_Colors[i])
			{
				all = false;
			}
		}
		if (all)
		{
			reset_renders();
		}
	}
	if (x - Platform_Size / 2.0 < 0)
	{
		if (possible(x - Cube_Size, z, world)) // Left
		{
			return true;
		}
		if (possible(x + Cube_Size, z, world)) // Right
		{
			return true;
		}
	}
	else
	{
		if (possible(x + Cube_Size, z, world)) // Right
		{
			return true;
		}
		if (possible(x - Cube_Size, z, world)) // Left
		{
			return true;
		}
	}
	if (z - Platform_Size / 2.0 < 0)
	{
		if (possible(x, z - Cube_Size, world)) // Back
		{
			return true;
		}
		if (possible(x, z + Cube_Size, world)) // Front
		{
			return true;
		}
	}
	else
	{
		if (possible(x, z + Cube_Size, world)) // Front
		{
			return true;
		}
		if (possible(x, z - Cube_Size, world)) // Back
		{
			return true;
		}
	}
	if (current.object_color)
	{
		bool all = true;
		for (int i = 0; i < Number_Of_Colors; ++i)
		{
			if (Render_Colors[i])
			{
				all = false;
			}
		}
		if (all)
		{
			for (int i = 0; i < Number_Of_Colors; ++i)
			{
				Render_Colors[i] = true;
			}
		}
		Render_Colors[(current.object_color - Color_Values) / 3] = false;
	}
	Path.pop_back();
	return false;
}

void update(int time, Object& self)
{

	static bool running = false;
	static Vertex old = Vertex(0, 0, 0);
	static int last = -1;

	static int total_fps = 0;
	static int number_fps = 0;

	float speed = .015 * time;
	static Camera& camera = Indigo::Current_World.camera;

	self.Is_Blank = true;

	if (Indigo::keys['q'])
	{
		running = !running;
		Indigo::keys['q'] = false;
	}

	if (Indigo::keys['e'])
	{
		reset_renders();
		self.Place(0.0, 0.75, 0.0);
		self.facing = Direction();
		old = Vertex(0.0, 0.75, 0.0);
		last = -1;
		camera.Place(0.0, 1.5, 0.0);
		Health = 100;
		Pause_Time = 100;
		Help_Index = 0;
		running = false;
	}

	if (Indigo::keys['r'])
	{
		initialize();
		return;
	}

	if (Indigo::keys['c'])
	{
		generate_colors();
		Indigo::keys['c'] = false;
	}

	if (Indigo::keys[','])
	{
		self.Y = 2.5 * Cube_Size;
	}

	static bool verbose = false;
	if (Indigo::keys['3'])
	{
		verbose = !verbose;
		Indigo::keys['3'] = false;
	}
	if (verbose)
	{
		int frame = 1000 / time;
		total_fps += frame;
		number_fps++;
		if (number_fps == 120)
		{
			number_fps = 1;
			total_fps = frame;
		}
		int average = total_fps / number_fps;
		static char * fps = new char[3];
		_itoa_s(average, fps, 3, 10);
		std::cout << fps[0] << fps[1] << fps[2] << std::endl;
		Indigo::Current_World.Add_Text(Text(-1.0, 0.8, fps, nullptr, GLUT_BITMAP_9_BY_15));
		std::cout << "Position: " << self.X << ", " << self.Y << ", " << self.Z << std::endl
			<< frame << " FPS" << std::endl << std::endl;
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
			Pause_Time += 0.0175 * time;
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
			reset_renders();
			self.Place(0.0, 0.75, 0.0);
			self.facing = Direction();
			old = Vertex(0.0, 0.75, 0.0);
			camera.Place(0.0, 1.5, 0.0);
			running = false;
			Pause_Time = 100;
			Help_Index = 0;
			Indigo::Current_World.Add_Text(Text(-0.1, 0.2, "You Lose!", nullptr, GLUT_BITMAP_9_BY_15, 120));
		}
	}
	else
	{
		if (Pause_Time > 0)
		{
			Pause_Time -= .04 * time;
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
	std::cout << self.facing.Get_X_Angle() << "-" << camera.eye.Get_X_Angle();
	if (Indigo::keys[' '])
	{
		self.Is_Blank = false;
		camera.Y += 20;
		self.facing.Normalize(10);
		camera.X += self.facing.Get_X() * -1;
		camera.Z += self.facing.Get_Z() * -1;
	}

	Object& collision = Indigo::Current_World.Get_Object(Indigo::Current_World.Collide(self, 0, -1));
	int color = (collision.object_color - Color_Values) / 3;
	int now = collision.ID;
	if (last != now && color >= 0 && color <= Number_Of_Colors - 1)
	{
		Render_Colors[color] = true;
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
			reset_renders();
		}
	}
	last = now;

	if (abs(self.X) > Platform_Size * Cube_Size / 2.0 || abs(self.Z) > Platform_Size * Cube_Size / 2.0)
	{
		Indigo::Current_World.Add_Text(Text(-0.1, 0.2, "You Win!", nullptr, GLUT_BITMAP_9_BY_15, 180, initialize));
		return;
	}

	old = Vertex(self.X, self.Y, self.Z);

	player = &self;

}

void sweep_camera(int time, Object& self)
{
	static int direction = 1;
	self.Is_Blank = true;
	self.facing = Direction(1, 0, 269.8);
	self.Y += 0.03 * time * direction;
	if (self.Y > 100 && direction == 1)
	{
		direction = -1;
	}
	if (self.Y <= 1.5 + 0.03 * time && direction == -1)
	{
		self.Y = 0.75;
		self.Update = update;
	}
	Indigo::Current_World.camera.Place(self.X, self.Y + 0.75, self.Z);
	Indigo::Current_World.camera.eye = self.facing;
}

void click(int button, int state, float x, float y)
{
	if (y < 0.1 && y > -0.4)
	{
		std::cout << "Setting player up.\n";
		Object& point = world.Get_Object(world.Add_Object(Object(0.0, 0.75, 0.0, Mesh::Load("Meshes\\Arrow.obj"), nullptr, 60, sweep_camera)));
		point.Set_Hitbox();
		delete player;
		player = &point;
		std::cout << "Playing. Switching to main world.\n";
		Indigo::Current_World = world;
		std::cout << "Performing last-minute changes.\n";
		Indigo::FPS_Mouse(true, player);
		Indigo::Mouse_Button_Function = nullptr;
		std::cout << "Initializing lighting state.\n";
		Indigo::Current_World.lighting.Set_Ambient(0.15);
		Direction light = Direction(1.0, 45.0, 45.0);
		Indigo::Current_World.lighting.Add_Light(light.Get_X(), light.Get_Y(), light.Get_Z(), true);
	}
	if (y < -0.4)
	{
		exit(0);
	}
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
	Indigo::Current_World.Add_Text(Text(-0.5, -0.8, "Beginning to load.", nullptr, GLUT_BITMAP_9_BY_15, -1));
	glutPostRedisplay();
	std::cout << "Initializing walls\n";
	srand(std::time(0));
	generate_colors();
	bool is_possible = false;
	while (!is_possible)
	{
		Path = std::vector<Vertex>();
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
		is_possible = possible(0, 0, world);
		reset_renders();
	}
	world.Add_Object(Object((Platform_Size / 2.0 + 0.5) * Cube_Size, Cube_Size * 1.5, 0, Mesh::Box(Cube_Size, Cube_Size, (Platform_Size + 2) * Cube_Size)));
	world.Add_Object(Object((Platform_Size / -2.0 - 0.5) * Cube_Size, Cube_Size * 1.5, 0, Mesh::Box(Cube_Size, Cube_Size, (Platform_Size + 2) * Cube_Size)));
	world.Add_Object(Object(0, Cube_Size * 1.5, (Platform_Size / 2.0 + 0.5) * Cube_Size, Mesh::Box(Platform_Size * Cube_Size, Cube_Size, Cube_Size)));
	world.Add_Object(Object(0, Cube_Size * 1.5, (Platform_Size / -2.0 - 0.5) * Cube_Size, Mesh::Box(Platform_Size * Cube_Size, Cube_Size, Cube_Size)));
	std::cout << "Setting up HUD.\n";
	for (int i = 0; i < Number_Of_Colors; ++i)
	{
		world.Add_Object(Object(0, 10, 0, Mesh::Cube(0.075), &Color_Values[i * 3], 60, show));
	}
	world.Add_2D_Object(Object(0.0, -0.65, 0.0, Mesh::Rectangle(1.5, 0.075), Indigo::Red_Color, 60, check_health));
	world.Add_Text(Text(-0.07, -0.66, "Health", Indigo::White_Color, GLUT_BITMAP_9_BY_15));
	world.Add_2D_Object(Object(0.0, -0.8, 0.0, Mesh::Rectangle(1.0, 0.025), nullptr, 60, pause));
	world.Add_Object(Object(0, 3, 0, Mesh::Sphere(2, 3), Indigo::Blue_Color, 60, show_path));
	std::cout << "Initializing camera.\n";
	world.camera.Place(0, 1.5, 0);
	std::cout << "Setting up menu world.\n";
	World menu;
	Mesh gui = Mesh::Rectangle(2, 2);
	gui.Texture("Textures\\Menu.bmp");
	menu.Add_2D_Object(Object(0, 0, 0, gui));
	Indigo::FPS_Mouse(false);
	Indigo::Mouse_Button_Function = click;
	std::cout << "Switching to menu world.\n";
	Indigo::Current_World = menu;
	std::cout << "Removing loading routine from queue.\n";
	Indigo::Update_Function = nullptr;
	std::cout << "Loaded. Next frame will run new world.\n";

}

int main(int argc, char ** argv)
{
	std::cout << "Initializing rendering environment.\n";
	Indigo::Initialize(argc, argv, "Dash", 60, true, Indigo::Sky_Color);
	initialize();
	std::cout << "Showing GUI for loading.\n";
	Indigo::Run();
	return 0;
}