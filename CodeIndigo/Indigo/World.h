// Defines a class for holding objects

#pragma once

#include <vector>
#include "Object.h"
#include "Lighting.h"
#include "Camera.h"
#include "Text.h"


class World
{
public:
	// Create a new, empty world
	World(void);
	// Copy a world
	World::World(const World& world);
	// Deletes the world, opens memory
	~World(void);

	// Updates every object in the world, prepares to render again
	void Update(const int& time);
	// Renders every object in the world
	void Render(void);
	// Adds an object to the world to be rendered and updated, returns an object ID
	int Add_Object(const Object& object);
	// Gets an object based on an index. DO NOT attempt to store the reference after a push_back. Ever.
	Object& Get_Object(const int& ID) const;
	// Removes an object from the world based on an object ID
	void Remove_Object(const int& ID);
	// Removes an object from the world based on object; gets ID automatically
	void Remove_Object(const Object& object);
  // Add text to the world to be rendered, returns no handle to the text
  void Add_Text(const Text& text);

	// Checks whether any object collides with another object, each collision testing returns -1 if no collision or object id for first
	int Collide(const Object& object, const float add_x = 0, const float add_y = 0, const float add_z = 0);
	// Checks whether any object will ever be intersected by a direction
	int Collide(const Direction& position, const Direction& direction);
	// Checks whether a vertex is within any object
	int Collide(const Vertex& vertex, const float add_x = 0, const float add_y = 0, const float add_z = 0);

	// Used to modify the lighting of the world
	Lighting lighting;
	// Camera used to look
	Camera camera;
protected:
	std::vector<Object> objects;
  std::vector<Text> texts;
};