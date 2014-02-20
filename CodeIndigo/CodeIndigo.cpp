#include "Indigo\IndigoEngine.h"
//#include "Load.h"
//#include "Menu.h"

Object player;

void go_up(int time, Object& self)
{
	self.Y += 0.0005 * time;
	Indigo::Current_World.View.Eye = player.Facing;
}

int main(int argc, char ** argv)
{
	Indigo::Initialize(argc, argv, "My test!");
	Indigo::Current_World.Light_Setup.Add_Light(0, 0, 0, false);
	Indigo::Current_World.Add_Object(Object(0, 0, 5, Mesh::Sphere(2), Indigo::Red_Color, go_up, "Textures\\test.bmp"));
	Indigo::FPS_Mouse(true, &player);
	Indigo::Run();
}

//int main(int argc, char ** argv)
//{
//
//	/*Indigo::Initialize(argc, argv, "Code Indigo");
//	Load::Set_Up_Load(Menu::Load);
//	Indigo::Run();*/
//	return 0;
//
//}