// Defines a class for holding objects

#include "World.h"
#include "Indigo.h"
#include <stdlib.h>
#include "glut.h"


// Create a new, empty world
World::World(void)
{
  objects = std::vector<Object>();
  texts = std::vector<Text>();
	lighting = Lighting();
  camera = Camera();
	return;
}


// Copy a world
World::World(const World& world)
{
	objects = world.objects;
  texts = world.texts;
  lighting = world.lighting;
  camera = world.camera;
	return;
}


// Deletes the world, opens memory
World::~World(void)
{
	return;
}


// Updates every object in the world, prepares to render again
void World::Update(const int& time)
{
	for (int Object_ID = 0; Object_ID<objects.size(); ++Object_ID)
	{
		if (objects[Object_ID].Update)
		{
			objects[Object_ID].Update(time, objects[Object_ID]);
		}
	}
	return;
}


// Renders every object in the world
void World::Render(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
	Indigo::Reshape();
	camera.Look();
	lighting.Position_Lights();
	for (int Object_ID = 0; Object_ID<objects.size(); ++Object_ID)
	{
		objects[Object_ID].Render();
	}
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glDisable(GL_DEPTH_TEST);
  for (int text = 0; text < texts.size(); ++text)
  {
    if (texts[text].Render())
    {
      texts.erase(texts.begin() + text);
    }
  }
	glutSwapBuffers();
	return;
}


// Adds an object to the world to be rendered and updated, returns an object ID
int World::Add_Object(const Object& object)
{
	objects.push_back(object);
	int Object_ID = objects.size() - 1;
	objects[Object_ID].ID = Object_ID;
	return Object_ID;
}


// Gets an object based on an index. DO NOT attempt to store the reference after a push_back. Ever.
Object& World::Get_Object(const int& ID) const
{
	return const_cast <Object&>(objects[ID]);
}


// Removes an object from the world based on an object ID
void World::Remove_Object(const int& ID)
{
	objects[ID] = Object();
	return;
}


// Removes an object from the world based on object; gets ID automatically
void World::Remove_Object(const Object& object)
{
	objects[object.ID] = Object();
	return;
}


// Add text to the world to be rendered, returns no handle to the text
void World::Add_Text(const Text& text)
{
  texts.push_back(text);
}


// Checks whether any object collides with another object, each collision testing returns -1 if no collision or object id for first
int World::Collide(const Object& object, const float add_x, const float add_y, const float add_z)
{
	for (int Object = 0; Object < objects.size(); ++Object)
	{
		if (Object != object.ID)
		{
			if (objects[Object].Collide(object, add_x, add_y, add_z))
			{
				return Object;
				break;
			}
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