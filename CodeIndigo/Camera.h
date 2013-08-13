// Allows the world to be viewed from different points

#pragma once

#include "Object.h"
#include "Direction.h"


class Camera
{
public:
	// Create a new camera by a position and a direction
	Camera (float x=0.0, float y=0.0, float z=0.0,
		Direction direction=Direction (),
		Direction above=Direction (1.0, 0.0, 90.0));
	// Create a camera based off another
	Camera (const Camera& camera);
	// Remove the camera and free memory
	~Camera (void);
	// Place the camera at a point
	void Place (float x, float y, float z);
	// Move relative to the way it's pointing and where it is
	void Move (float foward, float side=0.0);
	// Look in a direction
	void Look_At (Direction direction);
	// Look at a point
	void Look_At (float x, float y, float z);
	// Watch an object
	void Watch (Object object, Direction direction);
	// Look through the camera for this frame
	void Look (void) const;
	// Should be private, but copying.
	Direction up;
	float X;
	float Y;
	float Z;
	Direction eye;
private:
};