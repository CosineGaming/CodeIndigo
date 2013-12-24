// Defines a class for holding objects

#include "World.h"
#include "Indigo.h"
#include <stdlib.h>
#include "glut.h"


World::World(void)
{
	// Create a new, empty world
	lighting = Lighting();
	return;
}


World::World(const World& world)
{
	// Copy a world
	objects = world.objects;
	return;
}


World::~World(void)
{
	// Deletes the world, opens memory
	return;
}


void World::Update(const int& time)
{
	// Updates every object in the world, prepares to render again
	for (int Object_ID=0; Object_ID<objects.size(); ++Object_ID)
	{
		if (objects [Object_ID].Update)
		{
			objects [Object_ID].Update(time, objects [Object_ID]);
		}
	}
	return;
}


void World::Render(void)
{
	// Renders every object in the world
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Indigo::Reshape();
	camera.Look();
	lighting.Position_Lights();
	for (int Object_ID=0; Object_ID<objects.size(); ++Object_ID)
	{
		objects [Object_ID].Render();
	}
	glutSwapBuffers();
	return;
}


int World::Add_Object(const Object& object)
{
	// Adds an object to the world to be rendered and updated,
	// returns an object ID
	for (int Object_ID=0; Object_ID<objects.size(); ++Object_ID)
	{
		if (objects [Object_ID].Is_Blank)
		{
			objects [Object_ID] = object;
			objects [Object_ID].ID = Object_ID;
			return Object_ID;
		}
	}
	objects.push_back(object);
	int Object_ID = objects.size() - 1;
	objects [Object_ID].ID = Object_ID;
	return Object_ID;
}


Object& World::Get_Object(const int& ID) const
{
	return const_cast <Object&>(objects [ID]);
}


void World::Remove_Object(const int& ID)
{
	// Removes an object from the world based on an object ID
	objects [ID] = Object();
	return;
}


void World::Remove_Object(const Object& object)
{
	// Removes an object from the world based on object; gets ID automatically
	objects [object.ID] = Object();
	return;
}


// Checks whether any object collides with another object, each collision testing returns -1 if no collision or object id for first
int World::Collide(const Object& object, const float add_x, const float add_y, const float add_z)
{
	for (int Object = 0; Object < objects.size(); ++Object)
	{
		if (objects[Object].Collide(object, add_x, add_y, add_z))
		{
			return Object;
			break;
		}
	}
	return -1;
}


// Checks whether any object will ever be intersected by a direction
int World::Collide(const Direction& position, const Direction& direction)
{
	for (int Object = 0; Object < objects.size(); ++Object)
	{
		if (objects[Object].Collide(position, direction))
		{
			return Object;
		}
	}
	return -1;
}


// Checks whether a vertex is within any object
int World::Collide(const Vertex& vertex, const float add_x, const float add_y, const float add_z)
{
	for (int Object = 0; Object < objects.size(); ++Object)
	{
		if (objects[Object].Collide(vertex, add_x, add_y, add_z))
		{
			return Object;
		}
	}
	return -1;
}