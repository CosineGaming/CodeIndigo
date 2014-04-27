#include "Indigo/IndigoEngine.h"

Object Player;

Object Portal;

float Portal_Collide;
float Platform_Collide;
float Spike_Collide;

// The indices of the shader options
int Alternative_Shader;
int Normal_Shader;

float Gravity_Velocity;

const float Scroll_Speed = 0.0025;

void WSAD_Move(float speedtime)
{
	if (Indigo::Keys['w'])
	{
		Player.Move(speedtime);
	}
	if (Indigo::Keys['s'])
	{
		Player.Move(-speedtime);
	}
	if (Indigo::Keys['a'])
	{
		Player.Move(0, -speedtime);
	}
	if (Indigo::Keys['d'])
	{
		Player.Move(0, speedtime);
	}
}

void Update(float time)
{
	const float speed = 0.01;
	static Object checkpoint = Player;
	static Object total_death = Player;
	bool restarted = Player.Y < -100; // Died
	if (restarted)
	{
		if (Indigo::Current_World.Shader_Index == Alternative_Shader)
		{
			Player = checkpoint;
			Indigo::Current_World.View.Place(Player.X, Player.Y + 0.75, Player.Z);
			Gravity_Velocity = -0.0001;
		}
		else
		{
			Player = total_death;
			Indigo::Current_World.View.Place(Player.X, Player.Y + 0.75, Player.Z);
			Gravity_Velocity = -0.0001;
		}
	}
	WSAD_Move(speed*time);
	Indigo::Current_World.View.Eye = Player.Facing;
	Gravity_Velocity -= 0.0001 * time; // For some reason legit gravity doesn't look good
	if (Indigo::Keys[' '])
	{
		Object test = Player;
		test.Y += Gravity_Velocity * time;
		if (Indigo::Current_World.Collide(test) != -1 || Indigo::Keys['5'])
		{
			Gravity_Velocity = 0.02;
		}
		Indigo::Keys[' '] = false;
	}
	Player.Y += Gravity_Velocity * time;
	/*if (Player.Y < 0.75 && Indigo::Current_World.Shader_Index == Normal_Shader)
	{
		Player.Y = 0.75;
		Gravity_Velocity = 0;
	}*/
	if (Indigo::Keys['4']) // Cheat code
	{
		if (Indigo::Current_World.Shader_Index == Alternative_Shader)
		{
			Indigo::Current_World.Shader_Index = Normal_Shader;
		}
		else
		{
			Indigo::Current_World.Shader_Index = Alternative_Shader;
		}
		Indigo::Keys['4'] = false;
	}
	int collision = Indigo::Current_World.Collide(Player);
	if (collision != -1)
	{
		Object& with = Indigo::Current_World.Get_Object(collision);
		if (with.User_Data.size() && with.User_Data[0] == Portal_Collide)
		{
			if (Indigo::Current_World.Shader_Index == Alternative_Shader)
			{
				Indigo::Current_World.Shader_Index = Normal_Shader;
			}
			else
			{
				Indigo::Current_World.Shader_Index = Alternative_Shader;
			}
			while (Player.Collide(with))
			{
				WSAD_Move(speed*time*3);
				Player.Move(0.0001);
			}
			total_death = Player;
		}
		else
		{
			if (with.User_Data.size() && with.User_Data[0] == Spike_Collide)
			{
				Player.Y = -101;
				Indigo::Current_World.View.Y = Player.Y + 0.75;
			}
			if (Indigo::Current_World.Shader_Index == Alternative_Shader && (!with.User_Data.size() || !with.User_Data[0] == Platform_Collide))
			{
				// Not a platform, uh-oh!
				if (restarted)
				{
					Player.Place(0, 0.75, 0);
					Player.Facing = Direction();
					Indigo::Current_World.Shader_Index = Normal_Shader;
				}
				Indigo::Current_World.View.Place(Player.X, Player.Y + 0.75, Player.Z);
			}
			else
			{
				Object old_pos = Object(Indigo::Current_World.View.X, Indigo::Current_World.View.Y - 0.75, Indigo::Current_World.View.Z);
				Object check_pos = Object(Player.X, old_pos.Y, old_pos.Z);
				check_pos.Set_Hitbox(glm::vec3(-0.2, -0.75, -0.1), glm::vec3(0.2, 0.75, 0.1));
				if (Indigo::Current_World.Collide(check_pos) == -1)
				{
					Indigo::Current_World.View.X = Player.X;
				}
				else
				{
					Player.X = old_pos.X;
				}
				check_pos.Place(old_pos.X, old_pos.Y, Player.Z);
				if (Indigo::Current_World.Collide(check_pos) == -1)
				{
					Indigo::Current_World.View.Z = Player.Z;
				}
				else
				{
					Player.Z = old_pos.Z;
				}
				check_pos.Place(old_pos.X, Player.Y, old_pos.Z);
				if (Indigo::Current_World.Collide(check_pos) == -1)
				{
					Indigo::Current_World.View.Y = Player.Y + 0.75;
				}
				else
				{
					Player.Y = old_pos.Y;
					Gravity_Velocity = 0;
					if (Indigo::Current_World.Shader_Index == Alternative_Shader)
					{
						checkpoint = Player;
					}
				}
			}
		}
	}
	else
	{
		if (restarted)
		{
			Player = total_death;
			Indigo::Current_World.View.Place(Player.X, Player.Y + 0.75, Player.Z);
		}
		Indigo::Current_World.View.Place(Player.X, Player.Y + 0.75, Player.Z);
	}
}

void Scroll_X(float time, Object& self)
{
	int direction = self.User_Data[self.User_Data.size() - 1];
	self.X += direction * Scroll_Speed * time;
	if (self.Collide(Player, 0.0, 1) || self.Collide(Player))
	{
		Player.X += direction * Scroll_Speed * time;
	}
	if (Indigo::Current_World.Collide(self) != -1)
	{
		self.User_Data[self.User_Data.size() - 1] *= -1;
		self.X -= direction * Scroll_Speed * time;
	}
}

void Scroll_Y(float time, Object& self)
{
	int direction = self.User_Data[self.User_Data.size() - 1];
	self.Y += direction * Scroll_Speed * time;
	if (self.Collide(Player, 0.0, 2))
	{
		float old_y = Player.Y;
		Player.Y = self.Y + self.Data.Hitbox[1].y + 0.75;
		Indigo::Current_World.View.Y = Player.Y + 0.75;
	}
	if (Indigo::Current_World.Collide(self) != -1)
	{
		self.User_Data[self.User_Data.size() - 1] *= -1;
		self.Y -= direction * Scroll_Speed * time;
	}
}

void Scroll_Z(float time, Object& self)
{
	int direction = self.User_Data[self.User_Data.size() - 1];
	self.Z += direction * Scroll_Speed * time;
	if (self.Collide(Player, 0.0, 1) || self.Collide(Player))
	{
		Player.Z += direction * Scroll_Speed * time;
	}
	if (Indigo::Current_World.Collide(self) != -1)
	{
		self.User_Data[self.User_Data.size() - 1] *= -1;
		self.Z -= direction * Scroll_Speed * time;
	}
}

void Add_Platform(Object& object, float x, float y, float z, void (*function)(float time, Object& self)=nullptr)
{
	object.Update_Function = function;
	object.Place(x, y, z);
	Indigo::Current_World.Add_Object(object);
}

void Load(float time)
{
	static float begin = 0;
	if (begin == 0)
	{
		begin = time;
	}
	if (Indigo::Elapsed() - begin >= 200) // Wait before loading so the window can feel all happy
	{

		// Portal must be loaded first. If it weren't LD I'd find a way around this
		Portal.Place(0, 1, 10);
		Indigo::Current_World.Add_Object(Portal);
		Portal.Place(0, 14, -18);
		Indigo::Current_World.Add_Object(Portal);
		Portal.Place(1.5, 16, -30);
		Indigo::Current_World.Add_Object(Portal);
		Portal.Place(0, 0, -40);
		Indigo::Current_World.Add_Object(Portal);
		Portal.Place(13, 0, -52.5);
		Indigo::Current_World.Add_Object(Portal);


		// Room 0
		Object platform = Object(0, 1, 8, Mesh("Meshes/Platform.obj", "Textures/Platform.bmp"));
		Platform_Collide = 4.0; // ID for platform collisions
		platform.User_Data.push_back(Platform_Collide);
		platform.User_Data.push_back(1.0);
		Indigo::Current_World.Add_Object(platform);
		Add_Platform(platform, 0, 1, 9);
		Add_Platform(platform, 0, 1, 10);
		Add_Platform(platform, 0, 1, 11);
		Add_Platform(platform, 0, 1, 12);
		Add_Platform(platform, 0, 1, 13);

		// Symmetrical
		int neg = 1;
		for (int i = 0; i < 2; ++i)
		{
			Add_Platform(platform, neg * 1, 1, 13);
			Add_Platform(platform, neg * 3, 1, 13);
			Add_Platform(platform, neg * 4, 2, 13);
			Add_Platform(platform, neg * 6, 3, 13);
			Add_Platform(platform, neg * 9, 1, 13);
			Add_Platform(platform, neg * 11, 3, 13);
			Add_Platform(platform, neg * 14, 2, 13);
			Add_Platform(platform, neg * 13, 2, 12);
			Add_Platform(platform, neg * 13, 2, 11);
			Add_Platform(platform, neg * 13, 2, 10);
			Add_Platform(platform, neg * 13, 2, 9);
			Add_Platform(platform, neg * 13, 2, 6);
			Add_Platform(platform, neg * 13, 2, 5);
			Add_Platform(platform, neg * 13, 3, 2);
			Add_Platform(platform, neg * 13, 1, 2);
			Add_Platform(platform, neg * 13, 1, -1, Scroll_Z);
			Add_Platform(platform, neg * 13, 1, -8);
			Add_Platform(platform, neg * 13, 2, -9);
			Add_Platform(platform, neg * 13, 1, -11);
			Add_Platform(platform, neg * 12, 1, -12);
			Add_Platform(platform, neg * 11, 2, -12);
			Add_Platform(platform, neg * 9, 2, -12);
			Add_Platform(platform, neg * 5, 4, -12, Scroll_X);
			Add_Platform(platform, neg * 15, 4, -12); // Bounces off edge of wall
			neg *= -1;
		}

		// Room 1
		Add_Platform(platform, 0, 4, -12);
		Add_Platform(platform, 0, 2, -13);
		Add_Platform(platform, 0, 2, -14);
		Add_Platform(platform, 0, 2, -15);
		Add_Platform(platform, 0, -3, -16);
		Add_Platform(platform, 0, -2, -16, Scroll_Y);
		Add_Platform(platform, 0, 17, -16);
		Add_Platform(platform, 0, 14, -17);
		Add_Platform(platform, 0, 14, -18);
		Add_Platform(platform, 0, 14, -19);
		Add_Platform(platform, 0, 15, -21);
		Indigo::Current_World.Add_Object(Object(1, 15, -25.5, Mesh("Meshes/Ladder.obj", "Textures/Ladder.bmp")));
		// Portal Here
		Add_Platform(platform, 1.5, 16, -29);
		Add_Platform(platform, 1.5, 16, -30);
		Add_Platform(platform, 2, 12, -31);
		Add_Platform(platform, 2, 13, -31, Scroll_Y);
		Add_Platform(platform, 2, 23, -31);
		Add_Platform(platform, -2, 21, -31);
		Add_Platform(platform, -1, 21, -31, Scroll_X);
		Add_Platform(platform, 5, 21, -31);
		Add_Platform(platform, 2, 19, -32);
		Add_Platform(platform, 2, 20, -32, Scroll_Y);
		Add_Platform(platform, 2, 33, -32);
		Add_Platform(platform, 0, 28, -33);
		Add_Platform(platform, 1, 28, -33, Scroll_X);
		Add_Platform(platform, 4, 28, -33);
		Add_Platform(platform, 4, 22, -38);
		Add_Platform(platform, -3, 16, -38);
		Add_Platform(platform, -3, 10, -43);
		Add_Platform(platform, -2, 10, -43, Scroll_X);
		Add_Platform(platform, 10, 10, -43);
		Add_Platform(platform, 10, 10, -42);
		Add_Platform(platform, 10, 12, -40);
		Add_Platform(platform, 10, 16, -39);
		Add_Platform(platform, 10, 15, -39, Scroll_Y);
		Add_Platform(platform, 10, -1, -39);

		// Room 2
		Add_Platform(platform, 9, 0, -39);
		Add_Platform(platform, 6, 0, -39);
		Add_Platform(platform, 4, 0, -38);
		Add_Platform(platform, 1, 0, -38);
		Add_Platform(platform, 0, 0, -40);
		Add_Platform(platform, 0, 0, -41);
		Object spikes = Object(0, 0, 0, Mesh("Meshes/Spikes.obj", "Textures/Spikes.bmp"));
		Spike_Collide = 6.0;
		spikes.User_Data.push_back(Spike_Collide);
		spikes.User_Data.push_back(1.0);
		Add_Platform(spikes, 0, rand() % 8 + 1, -43, Scroll_Y);
		Add_Platform(spikes, 1, rand() % 8 + 1, -43, Scroll_Y);
		Add_Platform(spikes, 2, rand() % 8 + 1, -43, Scroll_Y);
		Add_Platform(spikes, 1, rand() % 8 + 1, -42, Scroll_Y);
		Add_Platform(spikes, 2, rand() % 8 + 1, -41, Scroll_Y);
		Add_Platform(spikes, 2, rand() % 8 + 1, -41, Scroll_Y);
		Add_Platform(spikes, 1, rand() % 8 + 1, -41, Scroll_Y);
		Add_Platform(spikes, 2, rand() % 8 + 1, -41, Scroll_Y);
		Add_Platform(spikes, 4, rand() % 8 + 1, -41, Scroll_Y);
		Add_Platform(spikes, 5, rand() % 8 + 1, -43, Scroll_Y);
		Add_Platform(spikes, 5, rand() % 8 + 1, -42, Scroll_Y);
		Add_Platform(spikes, 6, rand() % 8 + 1, -43, Scroll_Y);
		Add_Platform(spikes, 7, rand() % 8 + 1, -42, Scroll_Y);
		Add_Platform(spikes, 8, rand() % 8 + 1, -43, Scroll_Y);
		Add_Platform(spikes, 8, rand() % 8 + 1, -42, Scroll_Y);
		Add_Platform(spikes, 9, rand() % 8 + 1, -43, Scroll_Y);
		Add_Platform(spikes, 10, rand() % 8 + 1, -42, Scroll_Y);
		Add_Platform(spikes, 6, rand() % 8 + 1, -43, Scroll_Y);
		Add_Platform(spikes, 6, rand() % 8 + 1, -44, Scroll_Y);
		Add_Platform(spikes, 6, rand() % 8 + 1, -45, Scroll_Y);
		Add_Platform(spikes, 7, rand() % 8 + 1, -48, Scroll_Y);
		Add_Platform(spikes, 6, rand() % 8 + 1, -48, Scroll_Y);
		Add_Platform(spikes, 7, rand() % 8 + 1, -49, Scroll_Y);
		Add_Platform(spikes, 6, rand() % 8 + 1, -49, Scroll_Y);
		Add_Platform(spikes, 7, rand() % 8 + 1, -50, Scroll_Y);
		Add_Platform(spikes, 6, rand() % 8 + 1, -51, Scroll_Y);
		Add_Platform(spikes, 7, rand() % 8 + 1, -52, Scroll_Y);
		Add_Platform(spikes, 6, rand() % 8 + 1, -52, Scroll_Y);

		// Room 3
		Add_Platform(platform, 13, 0, -51.5);
		Add_Platform(platform, 13, 0, -52.5);
		Add_Platform(platform, 13, 0, -53.5);
		Add_Platform(platform, 13, 0, -55);
		Add_Platform(platform, 12, 0, -57);
		Add_Platform(platform, 11, 0, -59);
		Add_Platform(platform, 11, 0, -59);
		Add_Platform(platform, 12, 0, -59);
		Add_Platform(platform, 12, 0, -60);
		Add_Platform(platform, 12, 0, -61);
		Add_Platform(platform, 11, 0, -56);
		platform.User_Data[platform.User_Data.size()-1] = 4; // Super fast fast
		Add_Platform(platform, 11, 0, -61, Scroll_Z);
		Add_Platform(platform, 11, 0, -80);
		Add_Platform(platform, 11, 2, -78);
		Add_Platform(platform, 10, 0, -69);
		Add_Platform(platform, 10, 0, -70);
		Add_Platform(platform, 10, 0, -71);
		Add_Platform(platform, 10, 2, -53);
		Add_Platform(platform, 10, 2, -66, Scroll_Z);
		Add_Platform(platform, 10, 2, -120);
		Add_Platform(platform, 9, 0, -70, Scroll_X);
		Add_Platform(platform, -6, 0, -70);
		Add_Platform(platform, -6, 0, -71);
		Add_Platform(platform, -5, 0, -71);
		Add_Platform(platform, -4, 0, -71);
		Add_Platform(platform, -3, 0, -71);
		Add_Platform(platform, -7, 0, -70);
		Add_Platform(platform, -7, 0, -69);
		Add_Platform(platform, -7, 0, -68);
		Add_Platform(platform, -8, 0, -70);
		Add_Platform(platform, -9, 0, -70);
		Add_Platform(platform, -10, 0, -70);
		Add_Platform(platform, -25, 0, -69, Scroll_X);
		Add_Platform(platform, -37, 0, -69);
		Add_Platform(platform, -20, 0, -70);
		Add_Platform(platform, -25, 0, -70, Scroll_X);
		Add_Platform(platform, -50, 0, -70);
		Add_Platform(platform, -51, 0, -70);
		Add_Platform(platform, -50, 0, -71);
		Add_Platform(platform, -49, 0, -71);
		Add_Platform(platform, -48, 0, -71);
		platform.User_Data[platform.User_Data.size() - 1] = 1; // Back to normal

		// Invisible object that serves as a real world platform
		Object real_floor = Object();

		Indigo::Current_World.Add_Object(Object(0, 0, 0, Mesh("Meshes/Rooms/0.obj", "Textures/Rooms/0.bmp"), nullptr, Direction(), false));
		real_floor.Set_Hitbox(glm::vec3(-10, -1, -10), glm::vec3(10, 0, 10));
		Add_Platform(real_floor, 0, 0, 0);

		Indigo::Current_World.Add_Object(Object(0, -0.3, -29, Mesh("Meshes/Rooms/1.obj", "Textures/Rooms/1.bmp"), nullptr, Direction(), false));
		// Slipper roof ^ never supports

		Indigo::Current_World.Add_Object(Object(10, 0, -50, Mesh("Meshes/Rooms/2.obj", "Textures/Rooms/2.bmp"), nullptr, Direction(), false));
		real_floor.Set_Hitbox(glm::vec3(-12, 0, 6.5), glm::vec3(-5, 0, 10));
		Add_Platform(real_floor, 10, 0, -50);
		Add_Platform(real_floor, 10, 8, -50); // Need top for spikes to sway on
		real_floor.Set_Hitbox(glm::vec3(-5, 0, -2.5), glm::vec3(5, 0, 10));
		Add_Platform(real_floor, 10, 0, -50);
		Add_Platform(real_floor, 10, 8, -50);

		Indigo::Current_World.Add_Object(Object(10, 0, -70, Mesh("Meshes/Rooms/3.obj", "Textures/Rooms/3.bmp"), nullptr, Direction(), false));
		// TDODO: Add Supports

		//Indigo::Current_World.Add_2D_Object(Object(0, 0, 0, Mesh::Text("HI!")));
		Indigo::Current_World.Add_Object(Object(2, 5, 0, Mesh("Meshes/Crystal.obj", "Textures/Crystal.bmp")));

		Indigo::Update_Function = Update;
	}
}

int main()
{

	Indigo::Initialize("Ludum Dare 29", Indigo::Sky_Color);
	Indigo::Update_Function = Load;
	Indigo::FPS_Mouse(true, &Player);

	Player = Object(13, 0.75, -50);
	Player.Set_Hitbox(glm::vec3(-0.2, -0.75, -0.1), glm::vec3(0.2, 0.75, 0.1));
	Indigo::Current_World.View.Y = 1.5;
	Portal = Object(0, 0, 0, Mesh("Meshes/Portal.obj", "Textures/Portal.bmp", 10));
	std::cout << Portal.Data.Hitbox[0].x << "," << Portal.Data.Hitbox[0].y << "," << Portal.Data.Hitbox[0].z << " to " << Portal.Data.Hitbox[1].x << "," << Portal.Data.Hitbox[1].y << "," << Portal.Data.Hitbox[1].z << std::endl;
	Portal_Collide = 0.0; // ID for portal; used in collisions
	Portal.User_Data.push_back(Portal_Collide);
	Gravity_Velocity = 0;

	Indigo::Current_World.Shader("Experiment.vs", "Experiment.fs");
	Alternative_Shader = Indigo::Current_World.Shader_Index;
	Indigo::Current_World.Shader("Default.vs", "Default.fs");
	Normal_Shader = Indigo::Current_World.Shader_Index;
	Direction light_direction = Direction(1, 45, -45);
	Indigo::Current_World.Light_Setup.Set_Light(light_direction.Get_X(), light_direction.Get_Y(), light_direction.Get_Z(), true);

	Indigo::Run();

}
