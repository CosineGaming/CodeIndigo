#include "Direction.h"
#include "Vertex.h"


// Construct a new direction of 0, 0, 0
Direction::Direction(void)
{
	Set_Coordinates(1, 0, 0);
	zero = true;
}


// Construct a new direction based off of distance and 2 angles
Direction::Direction(const float in_distance, const float in_angle_x, const float in_angle_y)
{
	Set_Direction(in_distance, in_angle_x, in_angle_y);
	return;
}


// Copy a direction
Direction::Direction(const Direction& direction)
{
	Set_Coordinates(direction.Get_X(), direction.Get_Y(), direction.Get_Z());
}


// Construct a new direction based off of x, y, and z
Direction Direction::Coordinates(const float x, const float y, const float z)
{
	Direction construct;
	construct.Set_Coordinates(x, y, z);
	return construct;
}


// Destroy a direction
Direction::~Direction(void)
{
	return;
}


// Make the distance 1
void Direction::Normalize(const float unit)
{
	Set_Distance(unit);
}


// Dot product. Useful for lighting
float Direction::Dot(const Direction& direction) const
{
	return X * direction.Get_X()
		+ Y * direction.Get_Y() + Z * direction.Get_Z();
}


// Cross product. Finds perpendicular direction to plane defined by 2. Also useful for lighting and angles
Direction Direction::Cross(const Direction& direction)
{
	return Direction::Coordinates(
		Get_Y() * direction.Get_Z() - Get_Z() * direction.Get_Y(),
		Get_Z() * direction.Get_X() - Get_X() * direction.Get_Z(),
		Get_X() * direction.Get_Y() - Get_Y() * direction.Get_X());
}


float Direction::Get_X(void) const
{
	return zero ? 0 : X;
}

float Direction::Get_Y(void) const
{
	return zero ? 0 : Y;
}

float Direction::Get_Z(void) const
{
	return zero ? 0 : Z;
}


void Direction::Set_X(const float x)
{
	X = x;
	return;
}

void Direction::Set_Y(const float y)
{
	Y = y;
	return;
}

void Direction::Set_Z(const float z)
{
	Z = z;
	return;
}


float Direction::Get_Distance(void) const
{
	return zero ? 0 : sqrt(X*X + Y*Y + Z*Z);
}

float Direction::Get_X_Angle(void) const
{
	float x_angle = atan2(X, Z) * DEGREES_PER_RADIAN;
	return x_angle < 0 ? x_angle + 360 : x_angle;
}

float Direction::Get_Y_Angle(void) const
{
	float y_angle = atan2(Y, sqrt(X*X + Z*Z)) * DEGREES_PER_RADIAN;
	return y_angle < 0 ? y_angle + 360 : y_angle;
}


// Set the distance and angles of the direction individually
void Direction::Set_Distance(float distance)
{
	Set_Direction(distance, Get_X_Angle(), Get_Y_Angle());
}
void Direction::Set_X_Angle(float x_angle)
{
	Set_Direction(Get_Distance(), x_angle, Get_Y_Angle());
}
void Direction::Set_Y_Angle(float y_angle)
{
	Set_Direction(Get_Distance(), Get_X_Angle(), y_angle);
}


// Set the coordinates of the direction
void Direction::Set_Coordinates(const float x, const float y, const float z)
{
	if (x == 0 && y == 0 && z == 0)
	{
		zero = true;
	}
	else
	{
		X = x;
		Y = y;
		Z = z;
	}
	return;
}


// Add values to the coordinates of the direction
void Direction::Add_Coordinates(const float x, const float y, const float z)
{
	Set_Coordinates(X + x, Y + y, Z + z);
	return;
}


// Set the distance and angles
void Direction::Set_Direction(const float distance, const float angle_x, const float angle_y)
{
	float in_distance = distance;
	if (distance == 0)
	{
		in_distance = 1;
		zero = true;
	}
	else
	{
		zero = false;
	}
	float x_angle = angle_x / DEGREES_PER_RADIAN;
	float y_angle = angle_y / DEGREES_PER_RADIAN;
	Y = sin(y_angle) * in_distance;
	float partial = cos(y_angle) * in_distance;
	X = sin(x_angle) * partial;
	Z = cos(x_angle) * partial;
	return;
}


// Add values to the distance and angles
void Direction::Add_Direction(const float distance, const float angle_x, const float angle_y)
{
	Set_Direction(Get_Distance() + distance,
		Get_X_Angle() + angle_x,
		Get_Y_Angle() + angle_y);
	return;
}


// Find the distance between this direction and another
Direction Direction::Distance(const Direction& to) const
{
	Direction copy = to;
	copy.Add_Coordinates(-1 * X, -1 * Y, -1 * Z);
	return copy;
}


// Makes a vertex with the X, Y, and Z values
Vertex Direction::To_Vertex(void) const
{
	return Vertex(X, Y, Z);
}
