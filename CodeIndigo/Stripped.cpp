#include "Indigo/IndigoEngine.h"

int main()
{
	Indigo::Initialize("Crashes", Indigo::Sky_Color, 1280, 720, 24, false);
	Indigo::Current_World.Shader("CodeIndigo/Indigo/Shaders/Default.vs", "CodeIndigo/Indigo/Shaders/Default.fs");
	Indigo::Current_World.Add_2D_Object(Object(0, 0, 0, Mesh::Text("Static Object")));
	int handle = Indigo::Current_World.Add_2D_Object(Object(0, 0, 0, Mesh::Text("Original Object")));
	std::cout << handle << std::endl;
	Indigo::Current_World.Get_2D_Object(handle).Data = Mesh::Text("Changed Object");
	Indigo::Run();
	return 0;
}
