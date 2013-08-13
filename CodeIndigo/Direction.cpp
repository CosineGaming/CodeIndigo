#include "Direction.h"


Direction::Direction (float in_distance, float in_angle_x, float in_angle_y)
{
	Set_Direction (in_distance, in_angle_x, in_angle_y);
	return;
}


Direction::~Direction (void)
{
	return;
}


float Direction::Get_X (void) const
{
	return cos (angle_x) * cos (angle_y) * distance;
}


float Direction::Get_Y (void) const
{
	return sin (angle_y) * distance;
}


float Direction::Get_Z (void) const
{
	return sin (angle_x) * cos (angle_y) * distance;
}


float Direction::Get_Distance (void) const
{
	return distance;
}


float Direction::Get_X_Angle (void) const
{
	return angle_x * DEGREES_PER_RADIAN;
}


float Direction::Get_Y_Angle (void) const
{
	return angle_y * DEGREES_PER_RADIAN;
}


void Direction::Set_Coordinates (float x, float y, float z)
{
	float mid_direction = sqrt (z*z + x*x);
	angle_x = atan (z / x);
	angle_y = atan (y / mid_direction);
	distance = sqrt (mid_direction*mid_direction + y*y);
	return;
}


void Direction::Add_Coordinates (float x, float y, float z)
{
	float X = Get_X ();
	float Y = Get_Y ();
	float Z = Get_Z ();
	Set_Coordinates (X + x, Y + y, Z + z);
}


void Direction::Set_Direction (float in_distance, float in_angle_x, float in_angle_y)
{
	distance = in_distance / DEGREES_PER_RADIAN;
	angle_x = in_angle_x / DEGREES_PER_RADIAN;
	angle_y = in_angle_y / DEGREES_PER_RADIAN;
	return;
}


void Direction::Add_Direction (float in_distance, float in_angle_x, float in_angle_y)
{
	Set_Direction (distance + (in_distance / DEGREES_PER_RADIAN),
		angle_x + (in_angle_x / DEGREES_PER_RADIAN),
		angle_y + (in_angle_y / DEGREES_PER_RADIAN));
}