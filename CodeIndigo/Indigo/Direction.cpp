#include "Direction.h"
#include "Vertex.h"


// Construct a new direction of 0, 0, 0
Direction::Direction(void)
{
	Set_Coordinates(0, 0, 1);
	zero = true;
}


// Construct a new direction based off of distance and 2 angles
Direction::Direction(const float in_distance, const float in_angle_x, const float in_angle_y)
{
	Set_Coordinates(0, 0, 1);
	Set_Direction(in_distance, in_angle_x, in_angle_y);
	return;
}


// Copy a direction
Direction::Direction(const Direction& direction)
{
	Set_Coordinates(0, 0, 1);
	Set_Coordinates(direction.Get_X(), direction.Get_Y(), direction.Get_Z());
}


// Construct a new direction based off of x, y, and z
Direction Direction::Coordinates(const float x, const float y, const float z)
{
	Direction construct;
	construct.Set_Coordinates(0, 0, 1);
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
	if (false)//unit == 1.0)
	{
		float distance = Get_Distance();
		X /= distance;
		Y /= distance;
		Z /= distance;
	}
	else
	{
		Set_Distance(unit);
	}
}


// Dot product. Useful for lighting
float Direction::Dot(const Direction& direction) const
{
	return Get_X() * direction.Get_X()
		+ Get_Y() * direction.Get_Y() + Get_Z() * direction.Get_Z();
}

// Cross product. Finds perpendicular direction to plane defined by 2. Also useful for lighting and angles
Direction Direction::Cross(const Direction& direction) const
{
	return Direction::Coordinates(
		Get_Y() * direction.Get_Z() - Get_Z() * direction.Get_Y(),
		Get_Z() * direction.Get_X() - Get_X() * direction.Get_Z(),
		Get_X() * direction.Get_Y() - Get_Y() * direction.Get_X());
}

// Angle between two vectors, useful for rotation
float Direction::Angle_Distance(const Direction& direction) const
{
	if (!(X == direction.X && Y == direction.Y && Z == direction.Z))
	{
		return acos(Dot(direction) / Get_Distance() / direction.Get_Distance()) * DEGREES_PER_RADIAN;
	}
	return 0;
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
	if (x != 0)
	{
		zero = false;
		X = x;
	}
	else if (Y == 0 && Z == 0)
	{
		zero = true;
	}
	else
	{
		zero = false;
		X = x;
	}
	return;
}

void Direction::Set_Y(const float y)
{
	if (y != 0)
	{
		zero = false;
		Y = y;
	}
	else if (X == 0 && Z == 0)
	{
		zero = true;
	}
	else
	{
		zero = false;
		Y = y;
	}
	return;
}

void Direction::Set_Z(const float z)
{
	if (z != 0)
	{
		zero = false;
		Z = z;
	}
	else if (X == 0 && Y == 0)
	{
		zero = true;
	}
	else
	{
		zero = false;
		Z = z;
	}
	return;
}


float Direction::Get_Distance(void) const
{
	return sqrt(Get_X()*Get_X() + Get_Y()*Get_Y() + Get_Z()*Get_Z());
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
		zero = false;
	}
	return;
}


// Add values to the coordinates of the direction
void Direction::Add_Coordinates(const float x, const float y, const float z)
{
	Set_Coordinates(Get_X() + x, Get_Y() + y, Get_Z() + z);
	return;
}


// Set the distance and angles
void Direction::Set_Direction(const float distance, const float angle_x, const float angle_y)
{
	float in_distance = distance;
	if (distance == 0.0)
	{
		zero = true;
		return;
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
	copy.Add_Coordinates(-1 * Get_X(), -1 * Get_Y(), -1 * Get_Z());
	return copy;
}


// Makes a vertex with the X, Y, and Z values
Vertex Direction::To_Vertex(void) const
{
	return Vertex(Get_X(), Get_Y(), Get_Z());
}