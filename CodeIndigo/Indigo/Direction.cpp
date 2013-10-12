#include "Direction.h"
#include "Vertex.h"


// Construct a new direction based off of distance and 2 angles
Direction::Direction (float in_distance, float in_angle_x, float in_angle_y)
{
	Set_Direction (in_distance, in_angle_x, in_angle_y);
	return;
}


// Construct a new direction based off of x, y, and z
Direction Direction::Coordinates (float x, float y, float z)
{
	Direction construct;
	construct.Set_Coordinates (x, y, z);
	return (construct);
}


// Destroy a direction
Direction::~Direction (void)
{
	return;
}


// Make the distance 1
void Direction::Normalize (void)
{
	Set_Direction (1.0, Get_X_Angle (), Get_Y_Angle ());
}


// Dot product. Useful for lighting
float Direction::Dot (Direction direction) const
{
	Direction copy;
	copy.Normalize ();
	direction.Normalize ();
	return (copy.Get_X () * direction.Get_X ()
		+ copy.Get_Y () * direction.Get_Y () + copy.Get_Z () * direction.Get_Z ());
}


float Direction::Get_X (void) const
{
	return (sin (angle_x) * cos (angle_y) * distance);
}


float Direction::Get_Y (void) const
{
	return (sin (angle_y) * distance);
}


float Direction::Get_Z (void) const
{
	return (cos (angle_x) * cos (angle_y) * distance);
}


float Direction::Get_Distance (void) const
{
	return (distance);
}


float Direction::Get_X_Angle (void) const
{
	return (angle_x * DEGREES_PER_RADIAN);
}


float Direction::Get_Y_Angle (void) const
{
	return (angle_y * DEGREES_PER_RADIAN);
}


// Set the coordinates of the direction
void Direction::Set_Coordinates (float x, float y, float z)
{
	float mid_direction = sqrt (z*z + x*x);
	angle_x = atan2 (z, x);
	angle_y = atan2 (y, mid_direction);
	distance = sqrt (mid_direction*mid_direction + y*y);
	return;
}


// Add values to the coordinates of the direction
void Direction::Add_Coordinates (float x, float y, float z)
{
	float X = Get_X ();
	float Y = Get_Y ();
	float Z = Get_Z ();
	Set_Coordinates (X + x, Y + y, Z + z);
	return;
}


// Set the distance and angles
void Direction::Set_Direction (float in_distance, float in_angle_x, float in_angle_y)
{
	distance = in_distance;
	angle_x = fmod (in_angle_x / DEGREES_PER_RADIAN, 360.0 / DEGREES_PER_RADIAN);
	if (angle_x < 0)
	{
		angle_x += 360.0 / DEGREES_PER_RADIAN;
	}
	angle_y = fmod (in_angle_y / DEGREES_PER_RADIAN, 360.0 / DEGREES_PER_RADIAN);
	if (angle_y < 0)
	{
		angle_y += 360.0 / DEGREES_PER_RADIAN;
	}
	return;
}


// Add values to the distance and angles
void Direction::Add_Direction (float in_distance, float in_angle_x, float in_angle_y)
{
	Set_Direction (distance + in_distance,
		angle_x * DEGREES_PER_RADIAN + in_angle_x,
		angle_y * DEGREES_PER_RADIAN + in_angle_y);
	return;
}


// Find the distance between this direction and another
Direction Direction::Distance (Direction to)
{
	to.Add_Coordinates (-1 * Get_X (), -1 * Get_Y (), -1 * Get_Z ());
	return to;
}


// Makes a vertex with the X, Y, and Z values
Vertex Direction::To_Vertex (void) const
{
	return Vertex (Get_X (), Get_Y (), Get_Z ());
}