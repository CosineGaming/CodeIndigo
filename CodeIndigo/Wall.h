#pragma once
#include "C:\Users\Judah\Documents\GitHub\CodeIndigo\CodeIndigo\Indigo\Object.h"
class Wall : public Object
{
public:
	// Create a wall with a color index and a position
	Wall(int color, int x, int z);
	// Copy a wall
	Wall(Wall& wall);
	// Destroy a wall
	~Wall();
	// The color index of this object
	int Color;
	// The actual color for the object to prevent passing out of scope
	float pass[3];
private:
	// Converting color indices to real colors
	static const float Color_Values[15];
};

