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
	int collision = Indigo::Current_World.Collide(Player);
	Indigo::Current_World.View.Eye = Player.Facing;
	static float gravity_velocity = 0;
	gravity_velocity -= 0.0001 * time; // For some reason legit gravity doesn't look good
	if (Indigo::Keys[' '] && (Player.Y == 0.75 || collision != -1))
	{
		gravity_velocity = 0.02;
	}
	Player.Y += gravity_velocity * time;
	if (Player.Y < 0.75 && Indigo::Current_World.Shader_Index == Normal_Shader)
	{
		Player.Y = 0.75;
		gravity_velocity = 0;
	}
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
				Player.Y = Indigo::Current_World.View.Y - 0.75;
				while (Indigo::Current_World.Collide(Player) != -1)
				{
					WSAD_Move(speed*(time*=1.5));
					Player.Move(0.001);
				}
			}
		}
		else
		{
			std::cout << "Collide!\n";
			Object old_pos = Object(Indigo::Current_World.View.X, Indigo::Current_World.View.Y - 0.75, Indigo::Current_World.View.Z);
			Object check_pos = Object(Player.X, old_pos.Y, old_pos.Z);
			old_pos.Set_Hitbox(glm::vec3(-0.2, -0.75, -0.1), glm::vec3(0.2, 0.75, 0.1));
			std::cout << Player.Collide(with) << std::endl;
			if (!check_pos.Collide(with))
			{
				std::cout << "HA! Caught you, collide! I wasn't doing anything wrong!\n";
				Indigo::Current_World.View.X = Player.X;
			}
			else
			{
				Player.X = old_pos.X;
			}
			check_pos.Place(old_pos.X, Player.Y, old_pos.Z);
			if (!check_pos.Collide(with))
			{
				std::cout << "HA! Caught you, collide! I wasn't doing anything wrong!\n";
				Indigo::Current_World.View.Y = Player.Y + 0.75;
			}
			else
			{
				Player.Y = old_pos.Y;
				gravity_velocity = 0;
			}
			check_pos.Place(old_pos.X, old_pos.Y, Player.Z);
			if (!check_pos.Collide(with))
			{
				std::cout << "HA! Caught you, collide! I wasn't doing anything wrong!\n";
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

void Load(float time)
{
	static float begin = 0;
	if (begin == 0)
	{
		begin = time;
	}
	if (Indigo::Elapsed(begin) >= 200) // Wait before loading so the window can feel all happy
	{
		Portal.Place(0, 1, 9.5);
		Indigo::Current_World.Add_Object(Portal);
		Object platform = Object(0, 1, -12, Mesh("Meshes/Platform.obj", "Textures/Platform.bmp"));
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
	Portal.User_Data.push_back(1.0); // ID for portal; used in collisions
	Portal_Collide = 1.0;

	Indigo::Current_World.Shader("Experiment.vs", "Experiment.fs");
	Alternative_Shader = Indigo::Current_World.Shader_Index;
	Indigo::Current_World.Shader("Default.vs", "Default.fs");
	Normal_Shader = Indigo::Current_World.Shader_Index;
	Direction light_direction = Direction(1, 45, -45);
	Indigo::Current_World.Light_Setup.Set_Light(light_direction.Get_X(), light_direction.Get_Y(), light_direction.Get_Z(), true);

	Indigo::Run();

}
