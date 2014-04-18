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
	World(const World& world);
	// Deletes the world, opens memory
	~World(void);

	// Updates every object in the world, prepares to render again
	void Update(const float time);
	// Renders every object in the world
	void Render(void) const;

	// Compiles and puts in place custom Vertex or Fragment Shaders
	void Shader(const char * vertex, const char * fragment);

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

	// Adds a front object to the world to be rendered and updated, returns an object ID.
	// A front object is 3D, but doesn't get covered up by normal 3D objects. (Drawn in order of added)
	// A front object is also not affected by the view. The camera is assumed to look in the negative Z direction at 0,0,0.
	int Add_Front_Object(const Object& object);
	// Gets a front object based on an index. DO NOT attempt to store the reference after a push_back. Ever. Velociraptors.
	Object& Get_Front_Object(const int id) const;
	// Removes a front object from the world based on an object ID
	void Remove_Front_Object(const int id);
	// Removes a front object from the world based on object; gets ID automatically
	void Remove_Front_Object(const Object& object);

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
	int Collide(const glm::vec3& position, const Direction& direction);
	// Checks whether a vertex is within any object
	int Collide(const glm::vec3& vertex, const float add_x = 0, const float add_y = 0, const float add_z = 0);

	// Called every time the World updates, set this as you please
	void(*Update_Function)(const float time);

	// Used to modify the lighting arrangement of the world. Does not hold light normals, etc.
	Lighting Light_Setup;
	// The camera for looking in the world
	Camera View;

	// The handle for the full MVP matrix
	unsigned int Matrix_Handle;
	// The handle for the matrix defined by View.Look()
	unsigned int View_Matrix;
	// The handle for the matrix defined by each Object
	unsigned int Model_Matrix;
	// The handle for the full shader together.
	unsigned int Shader_Index;

private:

	// All the various Object-like things which the world stores
	std::vector<Object> objects;
	std::vector<Object> objects_front;
	std::vector<Object> objects_2d;
	std::vector<Text> texts;
	std::vector<Animation> animations;

	// A skybox is a box which is always in the back, textured to look like the game background
	Object skybox;

};