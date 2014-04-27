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
	if (Player.Y < -100) // Died
	{
		Player.Place(0, 0.75, 0);
		Player.Facing = Direction(1, 0, 0);
		Indigo::Current_World.Shader_Index = Normal_Shader;
	}
	int collision = Indigo::Current_World.Collide(Player);
	if (collision != -1)
	{
		Object& with = Indigo::Current_World.Get_Object(collision);
		if (with.User_Data.size())
		{
			if (with.User_Data[0] == Portal_Collide)
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
					WSAD_Move(speed*time*2);
					Player.Move(0.0001);
					std::cout << "try" << std::endl;
				}
			}
		}
		else
		{
			Object old_pos = Object(Indigo::Current_World.View.X, Indigo::Current_World.View.Y - 0.75, Indigo::Current_World.View.Z);
			Object check_pos = Object(Player.X, old_pos.Y, old_pos.Z);
			check_pos.Set_Hitbox(glm::vec3(-0.2, -0.75, -0.1), glm::vec3(0.2, 0.75, 0.1));
			//std::cout << Player.Y - 0.75 << " <- " << old_pos.Y - 0.75 << " < " << with.Y + with.Data.Hitbox[1].y << std::endl;
			if ((!check_pos.Collide(with)) && (Indigo::Current_World.Collide(check_pos) == -1))
			{
				Indigo::Current_World.View.X = Player.X;
			}
			else
			{
				Player.X = old_pos.X;
			}
			check_pos.Place(old_pos.X, Player.Y, old_pos.Z);
			if (!check_pos.Collide(with) && (Indigo::Current_World.Collide(check_pos) == -1))
			{
				Indigo::Current_World.View.Y = Player.Y + 0.75;
			}
			else
			{
				Player.Y = old_pos.Y;
				gravity_velocity = 0;
			}
			check_pos.Place(old_pos.X, old_pos.Y, Player.Z);
			if (!check_pos.Collide(with) && (Indigo::Current_World.Collide(check_pos) == -1))
			{
				Indigo::Current_World.View.Z = Player.Z;
			}
			else
			{
				Player.Z = old_pos.Z;
			}
		}
	}
	else
	{
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
	if (Indigo::Current_World.Collide(self) != -1)
	{
		self.User_Data[self.User_Data.size() - 1] *= -1;
		self.Z -= direction * 0.001 * time;
	}
}

void Add_Platform_At(Object& object, float x, float y, float z, void (*function)(float time, Object& self)=nullptr)
{
	object.Update_Function = function;
	object.Place(x, y, z);
	Indigo::Current_World.Add_Object(object);
	object.Update_Function = nullptr;
}

void Add_Platforms(void)
{
	Object platform = Object(0, 1, 8, Mesh("Meshes/Platform.obj", "Textures/Platform.bmp"));
	Indigo::Current_World.Add_Object(platform);
	Add_Platform_At(platform, 0, 1, 9);
	Add_Platform_At(platform, 0, 1, 10);
	Add_Platform_At(platform, 0, 1, 11);
	Add_Platform_At(platform, 0, 1, 12);
	Add_Platform_At(platform, 0, 1, 13);
	int neg = 1;
	for (int i = 0; i < 2; ++i)
	{
		Add_Platform_At(platform, neg * 1, 1, 13);
		Add_Platform_At(platform, neg * 3, 1, 13);
		Add_Platform_At(platform, neg * 4, 2, 13);
		Add_Platform_At(platform, neg * 6, 3, 13);
		Add_Platform_At(platform, neg * 9, 1, 13);
		Add_Platform_At(platform, neg * 11, 3, 13);
		Add_Platform_At(platform, neg * 14, 2, 13);
		Add_Platform_At(platform, neg * 13, 2, 12);
		Add_Platform_At(platform, neg * 13, 2, 11);
		Add_Platform_At(platform, neg * 13, 2, 10);
		Add_Platform_At(platform, neg * 13, 2, 9);
		Add_Platform_At(platform, neg * 13, 2, 6);
		Add_Platform_At(platform, neg * 13, 2, 5);
		Add_Platform_At(platform, neg * 13, 3, 2);
		Add_Platform_At(platform, neg * 13, 1, 2);
		Add_Platform_At(platform, neg * 13, 1, -1, Scroll_Z);
		Add_Platform_At(platform, neg * 13, 1, -4);
		Add_Platform_At(platform, neg * 13, 2, -4);
		Add_Platform_At(platform, neg * 13, 2, -5);
		neg *= -1;
	}
}

void Load(float time)
{
	static float begin = 0;
	if (begin == 0)
	{
		begin = time;
	}
	if (Indigo::Elapsed(begin) >= 200) // Wait before loading so the window can feel all happy
	{
		Portal.Place(0, 1.2, 10);
		Indigo::Current_World.Add_Object(Portal);
		Add_Platforms();
		Indigo::Current_World.Add_Object(Object( 0, 0,   0, Mesh("Meshes/Rooms/0.obj", "Textures/Rooms/0.bmp"), nullptr, Direction(), false));
		Indigo::Current_World.Add_Object(Object( 0, 0, -20, Mesh("Meshes/Rooms/1.obj", "Textures/Rooms/1.bmp"), nullptr, Direction(), false));
		Indigo::Current_World.Add_Object(Object(10, 0, -50, Mesh("Meshes/Rooms/2.obj", "Textures/Rooms/2.bmp"), nullptr, Direction(), false));
		Indigo::Current_World.Add_Object(Object(10, 0, -70, Mesh("Meshes/Rooms/3.obj", "Textures/Rooms/3.bmp"), nullptr, Direction(), false));
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
	Portal.User_Data.push_back(0.0); // ID for portal; used in collisions
	Portal_Collide = 0.0;

	Indigo::Current_World.Shader("Experiment.vs", "Experiment.fs");
	Alternative_Shader = Indigo::Current_World.Shader_Index;
	Indigo::Current_World.Shader("Default.vs", "Default.fs");
	Normal_Shader = Indigo::Current_World.Shader_Index;
	Direction light_direction = Direction(1, 45, -45);
	Indigo::Current_World.Light_Setup.Set_Light(light_direction.Get_X(), light_direction.Get_Y(), light_direction.Get_Z(), true);

	Indigo::Run();

}
