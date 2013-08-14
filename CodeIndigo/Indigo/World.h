// Defines a class for holding objects

#pragma once

#include <vector>
#include "Object.h"
#include "Lighting.h"
#include "Camera.h"


class World
{
public:
	// Create a new, empty world
	World (void);
	// Copy a world
	World::World (const World& world);
	// Deletes the world, opens memory
	~World (void);
	// Updates every object in the world, prepares to render again
	void Update (const int& frame) const;
	// Renders every object in the world
	void Render (void) const;
	// Adds an object to the world to be rendered and updated,
	// returns an object ID
	int Add_Object (const Object& object);
	// Gets an object based on 
	Object& Get_Object (const int& ID) const;
	// Removes an object from the world based on an object ID
	void Remove_Object (const int& ID);
	// Removes an object from the world based on object; gets ID automatically
	void Remove_Object (const Object& object);
	// Used to modify the lighting of the world
	Lighting lighting;
	// Camera used to look
	Camera camera;
protected:
private:
	std::vector <Object> objects;
};