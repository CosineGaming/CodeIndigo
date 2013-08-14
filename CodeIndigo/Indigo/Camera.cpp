// Allows the world to be viewed from different points

#include "Camera.h"
#include <stdlib.h>
#include "glut.h"


Camera::Camera (float x, float y, float z, Direction direction, Direction above)
{
	// Create a new camera by a position and a direction
	Place (x, y, z);
	eye = direction;
	up = above;
	return;
}


Camera::Camera (const Camera& camera)
{
	// Create a camera based off another
	Place (camera.X, camera.Y, camera.Z);
	Look_At (camera.eye);
	up = camera.up;
}


Camera::~Camera (void)
{
	// Remove the camera and free memory
	return;
}


void Camera::Place (float x, float y, float z)
{
	// Place the camera at a point
	X = x;
	Y = y;
	Z = z;
	return;
}


void Camera::Move (float forward, float side)
{
	// Move relative to the way it's pointing and where it is
	Direction direction = eye;
	direction.Set_Direction (forward);
	X += direction.Get_X ();
	Y += direction.Get_Y ();
	Z += direction.Get_Z ();
	direction.Set_Direction (side, direction.Get_X_Angle() + 90.0);
	X += direction.Get_X ();
	Y += direction.Get_Y ();
	Z += direction.Get_Z ();
}


void Camera::Look_At (Direction direction)
{
	// Look in a direction
	eye = direction;
	return;
}


void Camera::Look_At (float x, float y, float z)
{
	// Look at a point
	eye.Set_Coordinates (x, y, z);
	return;
}


void Camera::Watch (Object object, Direction relative_camera_position)
{
	// Watch an object
	eye.Set_Coordinates (object.X, object.Y, object.Z);
	Place (relative_camera_position.Get_X () + object.X,
		relative_camera_position.Get_Y () + object.Y,
		relative_camera_position.Get_Z () + object.Z);
	return;
}


void Camera::Look (void) const
{
	// Look through the camera for this frame
	glMatrixMode (GL_PROJECTION);
	gluLookAt (X, Y, Z, eye.Get_X (), eye.Get_Y (), eye.Get_Z (),
		up.Get_X (), up.Get_Y (), up.Get_Z ());
	glMatrixMode (GL_MODELVIEW);
	return;
}