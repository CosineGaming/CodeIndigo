#include "Wall.h"


// Create a wall with a color index and a position
Wall::Wall(int color, int x, int z) :
	Color(color),
	Object(x * 2 - 20.5, 1, z * 2 - 50.5, Mesh::Cube(1.9))
{
	pass[0] = Color_Values[color * 3];
	pass[1] = Color_Values[color * 3] + 1;
	pass[2] = Color_Values[color * 3] + 2;
	object_color = pass;
}


// Copy a wall
Wall::Wall(Wall& wall) :
	Color(wall.Color),
	Object(wall.X, 0.5, wall.Z, Mesh::Cube(1))
{
	pass[0] = Color_Values[wall.Color * 3];
	pass[1] = Color_Values[wall.Color * 3] + 1;
	pass[2] = Color_Values[wall.Color * 3] + 2;
	object_color = pass;
}


// Destroy a wall
Wall::~Wall()
{
}


// Converting color indices to real colors
const float Wall::Color_Values[15] = {
	0.8, 0.0, 0.0,
	1.0, 0.6, 0.0,
	0.0, 0.7, 0.7,
	0.0, 1.0, 0.0,
	0.0, 0.1, 0.6};