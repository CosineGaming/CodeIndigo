#include "Indigo/IndigoEngine.h"

Object Player;

Object Portal;
float Portal_Collide;

// The indices of the shader options
int Alternative_Shader;
int Normal_Shader;

void WSAD_Move(float speed, float time)
{
	if (Indigo::Keys['w'])
	{
		Player.Move(speed * time);
	}
	if (Indigo::Keys['s'])
	{
		Player.Move(-speed * time);
	}
	if (Indigo::Keys['a'])
	{
		Player.Move(0, -speed * time);
	}
	if (Indigo::Keys['d'])
	{
		Player.Move(0, speed * time);
	}
}

void Update(float time)
{
	const float speed = 0.01;
	WSAD_Move(speed, time);
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
				while (Indigo::Current_World.Collide(Player) != -1)
				{
					WSAD_Move(speed, time);
				}
			}
		}
	}
	Indigo::Current_World.View.Place(Player.X, Player.Y + 0.75, Player.Z);
	Indigo::Current_World.View.Eye = Player.Facing;
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
		Indigo::Current_World.Add_Object(Portal);
		Indigo::Update_Function = Update;
	}
}

int main()
{

	Player = Object(0, 0.75, 0);
	Portal = Object(0, 0, 0, Mesh("Meshes/Portal.obj", "Textures/Portal.bmp", 10));
	Portal.User_Data.push_back(1.0); // ID for portal; used in collisions
	Portal_Collide = 1.0;

	Indigo::Initialize("Ludum Dare 29", Indigo::Sky_Color, 1366, 768, 60, false);
	Indigo::Update_Function = Load;
	Indigo::FPS_Mouse(true, &Player);

	Indigo::Current_World.Shader("Experiment.vs", "Experiment.fs");
	Alternative_Shader = Indigo::Current_World.Shader_Index;
	Indigo::Current_World.Shader("Default.vs", "Default.fs");
	Normal_Shader = Indigo::Current_World.Shader_Index;
	Direction light_direction = Direction(1, 45, -45);
	Indigo::Current_World.Light_Setup.Set_Light(light_direction.Get_X(), light_direction.Get_Y(), light_direction.Get_Z(), true);

	Indigo::Run();

}
