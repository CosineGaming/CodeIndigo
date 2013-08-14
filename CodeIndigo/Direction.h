#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#define DEGREES_PER_RADIAN (180 / M_PI)


class Direction
{
public:
	// Construct a new direction based off of distance and 2 angles
	Direction (float in_distance=0.0, float in_angle_x=0.0, float in_angle_y=0.0);
	// Destroy a direction
	~Direction (void);
	// Get the relative coordinates of the direction
	float Get_X (void) const;
	float Get_Y (void) const;
	float Get_Z (void) const;
	// Get the distance and angles of the direction
	float Get_Distance (void) const;
	float Get_X_Angle (void) const;
	float Get_Y_Angle (void) const;
	// Set the coordinates of the direction
	void Set_Coordinates (float x, float y=0.0, float z=0.0);
	// Add values to the coordinates of the direction
	void Add_Coordinates (float x, float y=0.0, float z=0.0);
	// Set the distance and angles
	void Set_Direction (float in_distance, float in_angle_x=0.0, float in_angle_y=0.0);
	// Add values to the distance and angles
	void Add_Direction (float in_distance, float in_angle_x=0.0, float in_angle_y=0.0);
private:
	// The distance stretched when getting coords (1 for a normalized direction)
	float distance;
	// The angle moving left and right, or rotating on the y axis
	float angle_x;
	// The angle moving up and down, or rotating on the x axis
	float angle_y;
};