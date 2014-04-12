#include "Indigo\IndigoEngine.h"
#include <iostream>

Object player = Object(0, 0.75, 0);

World progressive;

void run(int time)
{
	if (Indigo::Keys['w'])
	{
		player.Move(0.002 * time);
	}
	if (Indigo::Keys['s'])
	{
		player.Move(-0.002 * time);
	}
	if (Indigo::Keys['a'])
	{
		player.Move(0, -0.002 * time);
	}
	if (Indigo::Keys['d'])
	{
		player.Move(0, 0.002 * time);
	}
	Indigo::Current_World.View.Place(player.X, player.Y + 0.75, player.Z);
	Indigo::Current_World.View.Eye = player.Facing;
}

void steadyload(int time)
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

void load(int time)
{
	std::cout << "Loading OBJ" << std::endl;
	Mesh woman = Mesh::Load("C:/Users/Judah/Documents/Frost/human.obj");
	std::cout << "Initializing (just a speed test, will be repeated)" << std::endl;
	woman.Initialize(true);
	std::cout << "Texturing" << std::endl;
	woman.Texture("C:/Users/Judah/Documents/Frost/human.bmp");
	std::cout << "Ok, really adding. Don't measure, partially optimised." << std::endl;
	progressive.Add_Object(Object(0, 0, 0, woman));
	std::cout << "Now YOU, tunnel..." << std::endl;
	progressive.Add_Object(Object(0, 0, 0, Mesh::Load("C:/Users/Judah/Documents/Frost/SpawnTunnel.obj"), nullptr, nullptr, "C:/Users/Judah/Documents/Frost/SpawnTunnel.bmp"));
	std::cout << time << std::endl;
	Indigo::FPS_Mouse(true, &player);
	Indigo::Current_World.Light_Setup.Add_Light(0.0, 0.0, 0.0);
	Indigo::Update_Function = steadyload;
}

int main(int argc, char ** argv)
{
	std::cout << Indigo::Fast_Float(" -10.9458") << std::endl;
	Indigo::Initialize(argc, argv, "Indigo Engine Test");
	Indigo::Update_Function = load;
	Indigo::Run();
}