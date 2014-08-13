// Defines a class for holding objects

#pragma once

#include "Camera.h"
#include "Lighting.h"
#include "Object.h"

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
	void Render(void);

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
	// Returns the number of objects in the world, simply objects.size()
	int Number_Of_Objects(void) const;

	// Adds a front object to the world to be rendered and updated, returns an object ID.
	// A front object is 3D, but doesn't get covered up by normal 3D objects. (Drawn in order of added)
	// A front object is also not affected by the view. The camera is assumed to look in the negative Z direction at 0,0,0.
	int Add_Front_Object(const Object& object);
	// Gets a front object based on an index. DO NOT attempt to store the reference after a push_back. Ever. Velociraptors.
	Object& Get_Front_Object(const int id) const;
	// Removes a front object from the world based on an object ID
	void Remove_Front_Object(const int id);
	// Returns the number of texts in the world, simply objects_front.size()
	int Number_Of_Front_Objects(void) const;

	// Adds a 2D object to the world to be rendered and updated, returns an object ID
	int Add_2D_Object(const Object& object);
	// Gets a 2D object based on an index. DO NOT attempt to store the reference after a push_back. Ever.
	Object& Get_2D_Object(const int id) const;
	// Removes a 2D object from the world based on an object ID
	void Remove_2D_Object(const int id);
	// Returns the number of 2d objects in the world, simply objects_2d.size()
	int Number_Of_2D_Objects(void) const;

	// Add animation along with its nested animations to the world to be rendered, returns no handle to the text
	int Add_Animation(const Animation& animation);
	// Removes the animation (and nested ones) from the world based on an object ID
	void Remove_Animation(const int id);
	// Returns the number of texts in the world, simply animations.size()
	int Number_Of_Animations(void) const;

	// Checks whether any object collides with another object, each collision testing returns -1 if no collision or object id for first
	int Collide(const Object& object, const float add_x = 0, const float add_y = 0, const float add_z = 0) const;
	// Checks whether any object will ever be intersected by a direction
	int Collide(const glm::vec2& position = glm::vec2(0, 0), const glm::mat4& camera_position = glm::mat4(0)) const;
	// Checks whether a vertex is within any object
	int Collide(const glm::vec3& vertex, const float add_x = 0, const float add_y = 0, const float add_z = 0) const;

	// Called every time the World updates, set this as you please
	void (*Update_Function)(const float time);
	// Called before each render cycle. Set however you like
	void (*Pre_Render_Function)(void);
	// Called after each render cycle
	void (*Post_Render_Function)(void);

	// Used to modify the lighting arrangement of the world. Does not hold light normals, etc
	Lighting Light_Setup;
	// The cameras for looking in the world and different types of render
	std::vector<Camera> Views;

	// The handle for the full MVP matrix
	unsigned int Matrix_Handle;
	// The handle for the matrix defined by View.Look()
	unsigned int View_Matrix;
	// The handle for the matrix defined by each Object
	unsigned int Model_Matrix;

private:

	// All the various Object-like things which the world stores
	std::vector<Object> objects;
	std::vector<Object> objects_front;
	std::vector<Object> objects_2d;
	std::vector<Animation> animations;

	// A skybox is a box which is always in the back, textured to look like the game background
	Object skybox;

};
