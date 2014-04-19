#include "Indigo/IndigoEngine.h"
#include <iostream>

Object player = Object(0, 0.75, 0);

char * fps = new char[4];

void run(float time)
{
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
		hand.Facing.Add_Direction(0, 0.390 * direction * time);
		hand.Z -= 0.004 * direction * time;
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
			hand.Z = 0.3;
			hand.Facing.Set_X_Angle(3);
		}
	}
}

void load(float time)
{
	//Indigo::Current_World.Add_Object(Object(1, 0, 1, Mesh("C:/Users/Judah/Documents/Frost/human.obj", "C:/Users/Judah/Documents/Frost/human.bmp")));
	//Indigo::Current_World.Add_Object(Object(0, 0, 0, Mesh("C:/Users/Judah/Documents/Frost/SpawnTunnel.obj", "C:/Users/Judah/Documents/Frost/SpawnTunnel.bmp")));
	Indigo::Current_World.Add_Object(Object(100, -1.5, 0, Mesh("C:/Users/Judah/Documents/Frost/Crossing.obj", "C:/Users/Judah/Documents/Frost/Crossing.bmp")));
	Indigo::Current_World.Add_Front_Object(Object(-0.1, -0.16, 0.3, Mesh("C:/Users/Judah/Documents/Frost/Arm.obj", "C:/Users/Judah/Documents/Frost/Arm.bmp"), nullptr, Direction(1, 3, 15)));
	Indigo::FPS_Mouse(true, &player);
	//Indigo::Current_World.Add_Text(Text(-1.0, 0.8, fps, Indigo::White_Color));
	//Indigo::Current_World.Light_Setup.Add_Light(0.0, 10.0, 0.0);
	Indigo::Update_Function = run;
	std::cout << "Done loading." << std::endl;
}

int main()
{
	float color[3] = { 0, 0.192, 0.314 };
	Indigo::Initialize("Indigo Engine Test", color);
	Indigo::Update_Function = load;
	Indigo::Current_World.Shader("Default.vs", "Default.fs");
	Indigo::Run();
}