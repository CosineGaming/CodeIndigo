#include "Load.h"


// Setup loading world, which is used between scenes, etc.
Loader::Loader(void(*load_function)(const int time), bool startup)
{
	if (startup)
	{
		load_world.Update_Function = Wait;
	}
	else
	{
		load_world.Update_Function = load_function;
	}
	load_world.Add_2D_Object(Object(0, 0, 0, Mesh::Rectangle(2, 2), Indigo::Red_Color));
	Indigo::Current_World = load_world;
}