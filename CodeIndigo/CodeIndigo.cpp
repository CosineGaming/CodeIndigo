#include "Indigo\IndigoEngine.h"
#include <iostream>

Object player = Object(0, 0.75, 0);

World progressive;

char * fps = new char[4];

void run(float time)
{
	if (Indigo::Keys['w'])
	{
		player.Move(0.009 * time);
	}
	if (Indigo::Keys['s'])
	{
		player.Move(-0.009 * time);
	}
	if (Indigo::Keys['a'])
	{
		player.Move(0, -0.009 * time);
	}
	if (Indigo::Keys['d'])
	{
		player.Move(0, 0.009 * time);
	}
	Indigo::Current_World.View.Place(player.X, player.Y + 0.75, player.Z);
	Indigo::Current_World.View.Eye = player.Facing;
	_itoa_s(1000 / time, fps, 4, 10);
}

void steadyload(float time)
{
	static int i = 0;
	static int wait = 10;
	if (wait <= 0)
	{
		Indigo::Current_World.Add_Object(progressive.Get_Object(i));
		++i;
		if (i >= progressive.Number_Of_Objects())
		{
			Indigo::Update_Function = run;
		}
		wait = 10;
	}
	else
	{
		wait -= time;
	}
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
	progressive.Add_Object(Object(1, 0, 1, Mesh::Load("C:/Users/Judah/Documents/Frost/human.obj"), nullptr, nullptr, "C:/Users/Judah/Documents/Frost/human.bmp"));
	progressive.Add_Object(Object(0, 0, 0, Mesh::Load("C:/Users/Judah/Documents/Frost/SpawnTunnel.obj"), nullptr, nullptr, "C:/Users/Judah/Documents/Frost/SpawnTunnel.bmp"));
	progressive.Add_Object(Object(100, -1.5, 0, Mesh::Load("C:/Users/Judah/Documents/Frost/Crossing.obj"), nullptr, nullptr, "C:/Users/Judah/Documents/Frost/Crossing.bmp"));
	progressive.Add_Object(Object(5, 0, 0, Mesh::Cube(1.5)));
	Indigo::Current_World.Add_Front_Object(Object(-0.1, -0.16, 0.3, Mesh::Load("C:/Users/Judah/Documents/Frost/Arm.obj"), nullptr, strike, "C:/Users/Judah/Documents/Frost/Arm.bmp", true, Direction(1,3,15)));
	Indigo::FPS_Mouse(true, &player);
	_itoa_s(60, fps, 4, 10);
	Indigo::Current_World.Add_Text(Text(-1.0, 0.8, fps, Indigo::White_Color));
	Indigo::Current_World.Light_Setup.Add_Light(0.0, 10.0, 0.0);
	Indigo::Update_Function = steadyload;
}

int main()
{
	float color[3] = { 0, 0.192, 0.314 };
	Indigo::Initialize("Indigo Engine Test", 120, false, color, 800, 600);
	Indigo::Update_Function = load;
	Indigo::Run();
}