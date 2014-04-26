#include "Indigo/IndigoEngine.h"

Object player;

void update(float time)
{
	if (Indigo::Keys['s'])
	{
		player.Move(-0.001 * time, 0);
		std::cout << player.X << ", " << player.Y << ", " << player.Z << std::endl;
	}
	Indigo::Current_World.View.Place(player.X, player.Y, player.Z);
	Indigo::Current_World.View.Eye = player.Facing;
}

int main()
{
	Indigo::Initialize("Ludum Dare 29", Indigo::Sky_Color, 1366, 768, 60, false);
	Indigo::Current_World.Shader("Default.vs", "Default.fs");
	Indigo::Current_World.Light_Setup.Set_Light(0, 1, 0, true);
	Indigo::Current_World.Add_Object(Object(0, 0, 0, Mesh("C:/Users/Judah/Documents/Frost/Crossing.obj", "C:/Users/Judah/Documents/Frost/Crossing.bmp")));
	Indigo::Update_Function = update;
	Indigo::FPS_Mouse(true, &player);
	Indigo::Run();
}
