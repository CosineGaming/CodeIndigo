// Allows the world to be viewed from different points

#include "Camera.h"
#include "Indigo.h"

#include <stdlib.h>
#include <iostream>
#include "GLFW/glfw3.h"
#include "glm/vec3.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"


// Create a new camera by a position and a direction
Camera::Camera(const float x, const float y, const float z, const Direction looking, const Direction above, const int field_of_view_x)
{
	Place(x, y, z);
	Eye = looking;
	Up = above;
	Field_Of_View = field_of_view_x;
	return;
}


// Create a camera based off another
Camera::Camera(const Camera& camera)
{
	Place(camera.X, camera.Y, camera.Z);
	Look_At(camera.Eye);
	Up = camera.Up;
	return;
}



// Remove the camera and free memory
Camera::~Camera(void)
{
	return;
}


// Place the camera at a point
void Camera::Place(const float x, const float y, const float z)
{
	X = x;
	Y = y;
	Z = z;
	return;
}


// Move relative to the way it's pointing and where it is
void Camera::Move(const float forward, const float side, const float up)
{
	Direction direction = Eye;
	direction.Set_Direction(forward, direction.Get_X_Angle(), 0.0);
	X += direction.Get_X();
	Y += direction.Get_Y();
	Z += direction.Get_Z();
	direction.Set_Direction(side, direction.Get_X_Angle() + 90.0, 0.0);
	X += direction.Get_X();
	Y += direction.Get_Y();
	Z += direction.Get_Z();
	direction.Set_Direction(up, 0.0, 90.0);
	X += direction.Get_X();
	Y += direction.Get_Y();
	Z += direction.Get_Z();
	return;
}


// Look in a direction
void Camera::Look_At(const Direction& direction)
{
	Eye = direction.Distance(Direction::Coordinates(X, Y, Z));
	return;
}


// Look at a point
void Camera::Look_At(const float x, const float y, const float z)
{
	Eye.Set_Coordinates(x - X, y - Y, z - Z);
	return;
}


// Look in a direction
void Camera::Look_Towards(const Direction& direction)
{
	Eye = direction;
	return;
}


// Look at a point
void Camera::Look_Towards(const float x, const float y, const float z)
{
	Eye.Set_Coordinates(x, y, z);
	return;
}


// Look at an object, should be updated frame-ly to maintain accuracy
void Camera::Watch(const Object& object, const Direction& relative_camera_position)
{
	Direction position = object.Facing;
	position.Add_Direction(0.0, relative_camera_position.Get_X_Angle(), relative_camera_position.Get_Y_Angle());
	Place(position.Get_X() + object.X,
		position.Get_Y() + object.Y,
		position.Get_Z() + object.Z);
	Eye.Set_Coordinates(object.X - X, object.Y - Y, object.Z - Z);
	return;
}


// Set up the projection matrix for this frame
glm::mat4 Camera::Project(void) const
{
	return glm::perspective(Field_Of_View / Indigo::Aspect_Ratio, (float) Indigo::Aspect_Ratio, (float)0.01, (float)500.0);
}


// Set up the 2D orthogonal matrix for this frame
glm::mat4 Camera::Project_2D(void) const
{
	return glm::ortho(-1 * Indigo::Aspect_Ratio, Indigo::Aspect_Ratio, -1.0f, 1.0f, -1.0f, 1.0f);
}


// Look through the camera for this frame with a full transformation
glm::mat4 Camera::Look(void) const
{
	return glm::lookAt(glm::vec3(X, Y, Z), glm::vec3(X + Eye.Get_X(), Y + Eye.Get_Y(), Z + Eye.Get_Z()),
		glm::vec3(Up.Get_X(), Up.Get_Y(), Up.Get_Z()));
}


// Look through the camera, but at the position 0,0,0. Used for Skybox.
glm::mat4 Camera::Look_In_Place(void) const
{
	return glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(Eye.Get_X(), Eye.Get_Y(), Eye.Get_Z()),
		glm::vec3(Up.Get_X(), Up.Get_Y(), Up.Get_Z()));
}