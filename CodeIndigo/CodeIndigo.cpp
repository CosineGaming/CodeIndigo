#include "Indigo/IndigoEngine.h"
#include <iostream>

Object player = Object(0, 0.75, 0);

char * fps = new char[4];

unsigned short normal;
unsigned short trippy;

void run(float time)
{
	static bool weird = false;
	static int next = 0;
	if (Indigo::Pressed('`'))
	{
		Indigo::Close();
	}
	if (Indigo::Keys['w'])
	{
		player.Move(0.02 * time);
	}
	if (Indigo::Keys['s'])
	{
		player.Move(-0.02 * time);
	}
	if (Indigo::Keys['a'])
	{
		player.Move(0, -0.02 * time);
	}
	if (Indigo::Keys['d'])
	{
		player.Move(0, 0.02 * time);
	}
	if (Indigo::Elapsed() >= next)
	{
		weird = !weird;
		if (weird)
		{
			Indigo::Current_World.Shader_Index = trippy;
			next = Indigo::Elapsed() + rand() % 800;
		}
		else
		{
			Indigo::Current_World.Shader_Index = normal;
			next = Indigo::Elapsed() + rand() % 45000;
		}
	}
	Indigo::Current_World.View.Place(player.X, player.Y + 0.75, player.Z);
	Indigo::Current_World.View.Eye = player.Facing;
}

void strike(float time, Object& hand)
{
	static int countdown = 0;
	static float direction = 0;
	if (Indigo::Left_Mouse && !direction)
	{
		countdown = 80;
		direction = 1;
	}
	if (countdown > 0)
	{
		hand.Facing.Add_Direction(0, -0.25 * direction * time);
		countdown -= time;
	}
	else
	{
		if (direction == 1)
		{
			direction = -1;
			countdown = 80;
		}
		else if (direction == -1)
		{
			direction = 0;
			countdown = 0;
			hand.Facing.Set_X_Angle(50);
		}
	}
}

void load(float time)
{
	srand(time);
	//Indigo::Current_World.Add_Object(Object(1, 0, 1, Mesh("C:/Users/Judah/Documents/Frost/human.obj", "C:/Users/Judah/Documents/Frost/human.bmp")));
	Indigo::Current_World.Add_Object(Object(0, 0, 0, Mesh("C:/Users/Judah/Documents/Frost/SpawnTunnel.obj", "C:/Users/Judah/Documents/Frost/SpawnTunnel.bmp")));
	Indigo::Current_World.Add_Object(Object(100, -1.5, 0, Mesh("C:/Users/Judah/Documents/Frost/Crossing.obj", "C:/Users/Judah/Documents/Frost/Crossing.bmp")));
	//Indigo::Current_World.Add_Object(Object(30, 15, 9, Mesh("C:/Users/Judah/Documents/Frost/Arm.obj", "C:/Users/Judah/Documents/Frost/Arm.bmp")));
	//Indigo::Current_World.Add_Front_Object(Object(-0.4, -0.16, 0.15, Mesh("C:/Users/Judah/Documents/Frost/Arm.obj", "C:/Users/Judah/Documents/Frost/Arm.bmp"), strike, Direction(1, 50, 20)));
	Indigo::FPS_Mouse(true, &player);
	//Indigo::Current_World.Add_Text(Text(-1.0, 0.8, fps, Indigo::White_Color));
	//Indigo::Current_World.Light_Setup.Add_Light(0.0, 10.0, 0.0);
	Indigo::Update_Function = run;
	std::cout << "Done loading." << std::endl;
}

int main()
{
	float color[3] = { 0, 0.192, 0.314 };
	Indigo::Initialize("Indigo Engine Test", color, 960, 540, 16, false);
	Indigo::Update_Function = load;
	Indigo::Current_World.Shader("Default.vs", "Default.fs");
	normal = Indigo::Current_World.Shader_Index;
	Indigo::Current_World.Shader("Experiment.vs", "Experiment.fs");
	trippy = Indigo::Current_World.Shader_Index;
	Indigo::Current_World.Shader_Index = normal;
	Indigo::Current_World.Light_Setup.Set_Ambient(0.1);
	Indigo::Current_World.Light_Setup.Set_Light(0.0, -1.0, 0.0, true);
	Indigo::Run();
}
