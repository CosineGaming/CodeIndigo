#include "Indigo\IndigoEngine.h"
#include "Load.h"
#include "Menu.h"

int main(int argc, char ** argv)
{

	Indigo::Initialize(argc, argv, "Code Indigo");
	Load::Set_Up_Load(Menu::Load);
	Indigo::Run();
	return 0;

}