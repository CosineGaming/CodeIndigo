#include "Indigo/IndigoEngine.h"

Object player = Object(0,0.75,0);

void update(float time)
{
	const float speed = 0.01;
	if (Indigo::Keys['w'])
	{
		player.Move(speed * time);
	}
	if (Indigo::Keys['s'])
	{
		player.Move(-speed * time);
	}
	if (Indigo::Keys['a'])
	{
		player.Move(0, -speed * time);
	}
	if (Indigo::Keys['d'])
	{
		player.Move(0, speed * time);
	}
	Indigo::Current_World.View.Place(player.X, player.Y + 0.75, player.Z);
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
