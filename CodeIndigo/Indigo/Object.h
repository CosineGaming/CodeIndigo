// Graphical objects to be displayed on the screen,
// along with useful functions for drawing and warping them

#pragma once

#include "Mesh.h"
#include "Direction.h"

#include "glm/glm.hpp"
#include "glm/vec3.hpp"


class Object
{

public:

	// Create an object given optional position, a mesh, and whether the object should render in wireframe
	Object(const float x = 0.0, const float y = 0.0, const float z = 0.0, const Mesh& mesh = Mesh(), void(*update_function)(const float time, Object& self) = nullptr,
		const Direction& towards = Direction(1, 0, 0), const bool world_collide = true);
	// Copy an object
	Object(const Object& object);
	// Destroys the object
	~Object(void);

	// Updates the object, preparing for User-Defined Update_Function
	void Update(const float time);
	// Renders the object
	void Render(const glm::mat4& projection, const glm::mat4& view, const bool lighting=true) const;

	// Places the object at the X, Y, and Z coordinates
	void Place(const float x, const float y=0.0, const float z=0.0);
	// Moves the forward, side, and up based on the facing direction
	void Move(const float forward, const float side=0.0, const float up=0.0);

	// Checks whether this object collides with another object
	bool Collide(const Object& object, const float add_x=0, const float add_y=0, const float add_z=0) const;
	// Checks whether this object will ever be intersected by a direction
	bool Collide(const glm::vec3& position, const Direction& direction) const;
	// Checks whether this vertex is withing this object
	bool Collide(const glm::vec3& vertex, const float add_x = 0, const float add_y = 0, const float add_z = 0) const;
	// Changes the relative hitbox for preliminary collision, set to 0 to make it uncollidable
	void Set_Hitbox(const glm::vec3& least=glm::vec3(0,0,0), const glm::vec3& most=glm::vec3(0,0,0));

	// Add a custom argument to send to the shader for this object.
	void Shader_Argument(char * argument, float value);

	// Check if it's been fully initialized
	int Is_Blank;
	// An ID used for accessing and deleting the object
	int ID;
	// Called every time the World updates, set this as you please
	void(*Update_Function)(const float time, Object& self);
	// Called right before the Object renders, set this as you please
	void(*Render_Function)(void);
	// Relative position of points
	Mesh Data;
	// The Direction the object is facing. Rotates in rendering!
	Direction Facing;
	// Whether or not to collide when searching through World::Collide
	bool World_Collide;
	// Data for the user to define for whatever purpose.
	std::vector<float> User_Data;
	// Custom data can be send to the shaders!
	std::vector<char *> Shader_Argument_Names;
	std::vector<float> Shader_Arguments;
	// X, Y, and Z position of center. Z is a constant if 2D.
	float X;
	float Y;
	float Z;

private:

};