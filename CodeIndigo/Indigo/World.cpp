// Defines a class for holding objects

#include "World.h"
#include "Indigo.h"
#include <stdlib.h>
#include "glut.h"


World::World (void)
{
	// Create a new, empty world
	lighting = Lighting ();
	return;
}


World::World (const World& world)
{
	// Copy a world
	objects = world.objects;
	return;
}


World::~World (void)
{
	// Deletes the world, opens memory
	return;
}


void World::Update (const int& frame)
{
	// Updates every object in the world, prepares to render again
	for (int Object_ID=0; Object_ID<objects.size (); ++Object_ID)
	{
		if (objects [Object_ID].Update)
		{
			objects [Object_ID].Update (frame, objects [Object_ID]);
		}
	}
	return;
}


void World::Render (void) const
{
	// Renders every object in the world
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode (GL_MODELVIEW);
	camera.Look ();
	lighting.Position_Lights ();
	glutSolidTeapot (0.3); // DELETE
	for (int Object_ID=0; Object_ID<objects.size (); ++Object_ID)
	{
		const_cast <Object&> (objects [Object_ID]).Render ();
	}
	glFlush ();
	glutSwapBuffers ();
	return;
}


int World::Add_Object (const Object& object)
{
	// Adds an object to the world to be rendered and updated,
	// returns an object ID
	for (int Object_ID=0; Object_ID<objects.size (); ++Object_ID)
	{
		if (objects [Object_ID].Is_Blank)
		{
			objects [Object_ID] = object;
			objects [Object_ID].ID = Object_ID;
			return (Object_ID);
		}
	}
	objects.push_back (object);
	int Object_ID = objects.size () - 1;
	objects [Object_ID].ID = Object_ID;
	return (Object_ID);
}


Object& World::Get_Object (const int& ID) const
{
	return (const_cast <Object&> (objects [ID]));
}


void World::Remove_Object (const int& ID)
{
	// Removes an object from the world based on an object ID
	objects [ID] = Object ();
	return;
}


void World::Remove_Object (const Object& object)
{
	// Removes an object from the world based on object; gets ID automatically
	objects [object.ID] = Object ();
	return;
}