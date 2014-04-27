#include "Indigo/IndigoEngine.h"

Object Player;

Object Portal;
float Portal_Collide;

// The indices of the shader options
int Alternative_Shader;
int Normal_Shader;

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
	bool restarted = Player.Y < -100; // Died
	if (restarted)
	{
		Player = checkpoint;
		Indigo::Current_World.View.Place(Player.X, Player.Y + 0.75, Player.Z);
	}
	WSAD_Move(speed*time);
	Indigo::Current_World.View.Eye = Player.Facing;
	static float gravity_velocity = 0;
	gravity_velocity -= 0.0001 * time; // For some reason legit gravity doesn't look good
	if (Indigo::Keys[' '])
	{
		Object test = Player;
		test.Y += gravity_velocity * time;
		if ((Player.Y == 0.75 || Indigo::Current_World.Collide(test) != -1))
		{
			gravity_velocity = 0.02;
		}
		Indigo::Keys[' '] = false;
	}
	Player.Y += gravity_velocity * time;
	if (Player.Y < 0.75 && Indigo::Current_World.Shader_Index == Normal_Shader)
	{
		Player.Y = 0.75;
		gravity_velocity = 0;
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
				gravity_velocity = 0;
				checkpoint = Player;
			}
		}
	}
	else
	{
		if (restarted)
		{
			Player.Place(0, 0.75, 0);
			Indigo::Current_World.Shader_Index = Normal_Shader;
		}
		Indigo::Current_World.View.Place(Player.X, Player.Y + 0.75, Player.Z);
	}
}

void Scroll_X(float time, Object& self)
{
	if (!self.User_Data.size())
	{
		self.User_Data.push_back(1);
	}
	int direction = self.User_Data[self.User_Data.size() - 1];
	self.X += direction * 0.001 * time;
	if (self.Collide(Player, 0.0, 1) || self.Collide(Player))
	{
		Player.X += direction * 0.001 * time;
	}
	if (Indigo::Current_World.Collide(self) != -1)
	{
		self.User_Data[self.User_Data.size() - 1] *= -1;
		self.X -= direction * 0.001 * time;
	}
}

void Scroll_Y(float time, Object& self)
{
	if (!self.User_Data.size())
	{
		self.User_Data.push_back(1);
	}
	int direction = self.User_Data[self.User_Data.size() - 1];
	self.Y += direction * 0.001 * time;
	if (self.Collide(Player, 0.0, 1) || self.Collide(Player))
	{
		Player.Y += direction * 0.001 * time;
	}
	if (Indigo::Current_World.Collide(self) != -1)
	{
		self.User_Data[self.User_Data.size() - 1] *= -1;
		self.Y -= direction * 0.001 * time;
	}
}

void Scroll_Z(float time, Object& self)
{
	if (!self.User_Data.size())
	{
		self.User_Data.push_back(1);
	}
	int direction = self.User_Data[self.User_Data.size() - 1];
	self.Z += direction * 0.001 * time;
	if (self.Collide(Player, 0.0, 1) || self.Collide(Player))
	{
		Player.Z += direction * 0.001 * time;
	}
	if (Indigo::Current_World.Collide(self) != -1)
	{
		self.User_Data[self.User_Data.size() - 1] *= -1;
		self.Z -= direction * 0.001 * time;
	}
}

void Add_Platform(Object& object, float x, float y, float z, void (*function)(float time, Object& self)=nullptr)
{
	object.Update_Function = function;
	object.Place(x, y, z);
	Indigo::Current_World.Add_Object(object);
	object.Update_Function = nullptr;
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
		Portal.Place(0, 2, -14.4);
		Indigo::Current_World.Add_Object(Portal);


		// Room 0
		Object platform = Object(0, 1, 8, Mesh("Meshes/Platform.obj", "Textures/Platform.bmp"));
		Indigo::Current_World.Add_Object(platform);
		Add_Platform(platform, 0, 1, 9);
		Add_Platform(platform, 0, 1, 10);Add_Platform(platform, 0, 1, 11);
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
		// Portal between these cubes
		Add_Platform(platform, 0, 2, -15);

		Indigo::Current_World.Add_Object(Object( 0, 0,   0, Mesh("Meshes/Rooms/0.obj", "Textures/Rooms/0.bmp"), nullptr, Direction(), false));
		Indigo::Current_World.Add_Object(Object( 0, 0, -16.8, Mesh("Meshes/Rooms/1.obj", "Textures/Rooms/1.bmp"), nullptr, Direction(), false));
		Indigo::Current_World.Add_Object(Object(10, 0, -50, Mesh("Meshes/Rooms/2.obj", "Textures/Rooms/2.bmp"), nullptr, Direction(), false));
		Indigo::Current_World.Add_Object(Object(10, 0, -70, Mesh("Meshes/Rooms/3.obj", "Textures/Rooms/3.bmp"), nullptr, Direction(), false));

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

	Player = Object(0, 0.75, 0);
	Player.Set_Hitbox(glm::vec3(-0.2, -0.75, -0.1), glm::vec3(0.2, 0.75, 0.1));
	Portal = Object(0, 0, 0, Mesh("Meshes/Portal.obj", "Textures/Portal.bmp", 10));
	std::cout << Portal.Data.Hitbox[0].x << "," << Portal.Data.Hitbox[0].y << "," << Portal.Data.Hitbox[0].z << " to " << Portal.Data.Hitbox[1].x << "," << Portal.Data.Hitbox[1].y << "," << Portal.Data.Hitbox[1].z << std::endl;
	Portal_Collide = 0.0; // ID for portal; used in collisions
	Portal.User_Data.push_back(Portal_Collide);

	Indigo::Current_World.Shader("Experiment.vs", "Experiment.fs");
	Alternative_Shader = Indigo::Current_World.Shader_Index;
	Indigo::Current_World.Shader("Default.vs", "Default.fs");
	Normal_Shader = Indigo::Current_World.Shader_Index;
	Direction light_direction = Direction(1, 45, -45);
	Indigo::Current_World.Light_Setup.Set_Light(light_direction.Get_X(), light_direction.Get_Y(), light_direction.Get_Z(), true);

	Indigo::Run();

}
