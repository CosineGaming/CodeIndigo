// Allows the world to be viewed from different points

#pragma once

#include "Object.h"
#include "Direction.h"

#include "glm\glm.hpp"
#include "glm\mat4x4.hpp"


class Camera
{

public:

	// Create a new camera by a position and a direction
	Camera(const float x = 0.0, const float y = 0.0, const float z = 0.0,
		const Direction looking=Direction(1.0),
		const Direction above = Direction(1.0, 0.0, 90.0), const int field_of_view = 90);
	// Create a camera based off another
	Camera(const Camera& camera);
	// Remove the camera and free memory
	~Camera(void);

	// Place the camera at a point
	void Place(const float x, const float y, const float z);
	// Move relative to the way it's pointing and where it is
	void Move(const float foward, const float side = 0.0, const float up = 0.0);
	// Look in a direction
	void Look_At(const Direction& direction);
	// Look at a point
	void Look_At(const float x, const float y, const float z);
	// Look in a direction relative to the camera
	void Look_Towards(const Direction& direction);
	// Look at a point relative to the camera
	void Look_Towards(const float x, const float y, const float z);
	// Look at an object, should be updated frame-ly to maintain accuracy
	void Watch(const Object& object, const Direction& direction);

	// Whenever the window resizes / initializes or FOV changes,
	// correct aspect ratio and angle for Projection matrix and store
	void Project(void);

	// Look through the camera for this frame with a full transformation, actually executed by [I200: World, I300: Fragment Shader]
	glm::mat4 Look(void) const;
	// Look through the camera, but at the position 0,0,0. Used for Skybox.
	glm::mat4 Look_In_Place(void) const;

	// The direction of the camera that is up.
	Direction Up;
	// The camera's position
	float X;
	float Y;
	float Z;
	// The way the camera is looking
	Direction Eye;
	// The angle of the field of view on the x axis
	int Field_Of_View;
	// The last updated projection matrix
	glm::mat4 Projection;

private:

};