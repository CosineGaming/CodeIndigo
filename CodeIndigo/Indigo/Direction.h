#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#define DEGREES_PER_RADIAN (180 / M_PI)

class Vertex;


class Direction
{
public:
	// Construct a new direction of 0, 0, 0
	Direction(void);
	// Construct a new direction based off of distance and 2 angles
	Direction(const float in_distance, const float in_angle_x = 0.0, const float in_angle_y = 0.0);
	// Copy a direction
	Direction(const Direction& direction); 
	// Construct a new direction based off of x, y, and z
	static Direction Coordinates(const float x, const float y, const float z);
	// Destroy a direction
	~Direction(void);
	// Make the distance 1
	void Normalize(const float unit=1.0);
	// Dot product. Useful for lighting
	float Dot(const Direction& direction) const;
	// Get the relative coordinates of the direction
	float Get_X(void) const;
	float Get_Y(void) const;
	float Get_Z(void) const;
	// Set the relative coordinates of the direction individually
	void Set_X(const float x);
	void Set_Y(const float y);
	void Set_Z(const float z);
	// Get the distance and angles of the direction
	float Get_Distance(void) const;
	float Get_X_Angle(void) const;
	float Get_Y_Angle(void) const;
	// Set the distance and angles of the direction individually
	void Set_Distance(float distance);
	void Set_X_Angle(float x_angle);
	void Set_Y_Angle(float y_angle);
	// Set the coordinates of the direction
	void Set_Coordinates(const float x, const float y, const float z);
	// Add values to the coordinates of the direction
	void Add_Coordinates(const float x, const float y=0.0, const float z=0.0);
	// Set the distance and angles
	void Set_Direction(const float distance, const float angle_x, const float angle_y);
	// Add values to the distance and angles
	void Add_Direction(const float distance, const float angle_x=0.0, const float angle_y=0.0);
	// Find the distance between this direction and another
	Direction Distance(const Direction& to) const;
	// Makes a vertex with the X, Y, and Z values
	Vertex To_Vertex(void) const;
private:
	// The distance stretched when getting coords(1 for a normalized direction)
	float X;
	// The angle moving left and right, or rotating on the y axis
	float Y;
	// The angle moving up and down, or rotating on the x axis
	float Z;
	// If distance is set to zero, don't actually change values. Instead set this flag that says to return all zeros
	bool zero;
};