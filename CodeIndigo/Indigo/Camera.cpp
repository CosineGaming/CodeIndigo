// Allows the world to be viewed from different points

#include "Camera.h"
#include <stdlib.h>
#include "glut.h"


Camera::Camera(float x, float y, float z, Direction direction, Direction above)
{
	// Create a new camera by a position and a direction
	Place(x, y, z);
	eye = direction;
	up = above;
	return;
}


Camera::Camera(const Camera& camera)
{
	// Create a camera based off another
	Place(camera.X, camera.Y, camera.Z);
	Look_At(camera.eye);
	up = camera.up;
}


Camera::~Camera(void)
{
	// Remove the camera and free memory
	return;
}


void Camera::Place(float x, float y, float z)
{
	// Place the camera at a point
	X = x;
	Y = y;
	Z = z;
	return;
}


void Camera::Move(float forward, float side, float up)
{
	// Move relative to the way it's pointing and where it is
	Direction direction = eye;
	direction.Set_Direction(forward, direction.Get_X_Angle(), 0.0);
	X += direction.Get_X();
	Y += direction.Get_Y();
	Z += direction.Get_Z();
	direction.Set_Direction(side, direction.Get_X_Angle() + 90.0, 0.0);
	X += direction.Get_X();
	Y += direction.Get_Y();
	Z += direction.Get_Z();
	direction.Set_Direction(up, direction.Get_X_Angle(), 90.0);
	X += direction.Get_X();
	Y += direction.Get_Y();
	Z += direction.Get_Z();
}


// Look in a direction
void Camera::Look_At(Direction direction)
{
	eye = direction.Distance(Direction::Coordinates(X, Y, Z));
	eye.Normalize();
	return;
}


// Look at a point
void Camera::Look_At(float x, float y, float z)
{
	eye.Set_Coordinates(x - X, y - Y, z - Z);
	eye.Normalize();
	return;
}


// Look in a direction
void Camera::Look_Towards(Direction direction)
{
	eye = direction;
	return;
}


// Look at a point
void Camera::Look_Towards(float x, float y, float z)
{
	eye.Set_Coordinates(x, y, z);
	return;
}


// Look at an object
void Camera::Watch(Object object, Direction relative_camera_position)
{
	Place(relative_camera_position.Get_X() + object.X,
		relative_camera_position.Get_Y() + object.Y,
		relative_camera_position.Get_Z() + object.Z);
	eye.Set_Coordinates(object.X - X, object.Y - Y, object.Z - Z);
	return;
}


void Camera::Look(void) const
{
	// Look through the camera for this frame
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(X, Y, Z, X + eye.Get_X(), Y + eye.Get_Y(), Z + eye.Get_Z(),
		up.Get_X(), up.Get_Y(), up.Get_Z());
	return;
}