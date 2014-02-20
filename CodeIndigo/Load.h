#pragma once

#include "Indigo\IndigoEngine.h"

class Loader
{
public:

	Loader(void(*load_function)(const int time), bool startup = false);
	void Load(void);

private:

	void Wait(const int time);

	World load_world;
	void(*Load_Function)(const int time)

}