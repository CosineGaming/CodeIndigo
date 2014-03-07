// Defines a class for holding objects

#pragma once

#include "Camera.h"
#include "Lighting.h"
#include "Object.h"
#include "Text.h"

#include <vector>


class Animation;


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
	void Update(const int time);
	// Renders every object in the world
	void Render(void) const;

	/*
	Add a skybox, a backdrop to the world. Accepts a filename of a texture.
	The texture will be in the format of six square textures arranged like this:
	     -----
	     |Top|
	-----------------------
	|Left|Front|Right|Back|
	-----------------------
	    |Bottom|
	    -------------
	Set to nullptr to remove the skybox.
	*/
	void Add_Skybox(const char * texture);

	// Adds an object to the world to be rendered and updated, returns an object ID
	int Add_Object(const Object& object);
	// Gets an object based on an index. DO NOT attempt to store the reference after a push_back. Ever.
	Object& Get_Object(const int id) const;
	// Removes an object from the world based on an object ID
	void Remove_Object(const int id);
	// Removes an object from the world based on object; gets ID automatically
	void Remove_Object(const Object& object);

	// Adds a 2D object to the world to be rendered and updated, returns an object ID
	int Add_2D_Object(const Object& object);
	// Gets a 2D object based on an index. DO NOT attempt to store the reference after a push_back. Ever.
	Object& Get_2D_Object(const int id) const;
	// Removes a 2D object from the world based on an object ID
	void Remove_2D_Object(const int id);
	// Removes a 2D object from the world based on object; gets ID automatically
	void Remove_2D_Object(const Object& object);

	// Add text to the world to be rendered, returns no handle to the text
	int Add_Text(const Text& text);
	// Gets text based on an index. DO NOT attempt to store the reference after a push_back. Ever.
	Text& Get_Text(const int id) const;
	// Removes text from the world based on an object ID
	void Remove_Text(const int id);
	// Removes text from the world based on text; gets ID automatically
	void Remove_Text(const Text& text);

	// Add animation along with its nested animations to the world to be rendered, returns no handle to the text
	int Add_Animation(const Animation& animation);
	// Removes the animation (and nested ones) from the world based on an object ID
	void Remove_Animation(const int id);
	// Removes animation from the world based on animation itself; gets ID automatically
	void Remove_Animation(const Animation& animation);

	// Returns the number of objects in the world, simply objects.size()
	int Number_Of_Objects(void);
	// Returns the number of 2d objects in the world, simply objects_2d.size()
	int Number_Of_2D_Objects(void);
	// Returns the number of texts in the world, simply texts.size()
	int Number_Of_Texts(void);

	// Checks whether any object collides with another object, each collision testing returns -1 if no collision or object id for first
	int Collide(const Object& object, const float add_x = 0, const float add_y = 0, const float add_z = 0);
	// Checks whether any object will ever be intersected by a direction
	int Collide(const Direction& position, const Direction& direction);
	// Checks whether a vertex is within any object
	int Collide(const Vertex& vertex, const float add_x = 0, const float add_y = 0, const float add_z = 0);

	// Called every time the World updates, set this as you please
	void(*Update_Function)(const int time);

	// Used to modify the lighting arrangement of the world. Does not hold light normals, etc.
	Lighting Light_Setup;
	// The camera for looking in the world
	Camera View;

private:

	std::vector<Object> objects;
	std::vector<Object> objects_2d;
	std::vector<Text> texts;
	std::vector<Animation> animations;

	Object skybox;

};