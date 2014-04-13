// Defines a class for holding objects

#include "World.h"

#include "Animation.h"
#include "Indigo.h"

#include <stdlib.h>
#include "glut.h"


// Create a new, empty world
World::World(void)
{
	objects = std::vector<Object>();
	objects_front = std::vector<Object>();
	objects_2d = std::vector<Object>();
	texts = std::vector<Text>();
	Light_Setup = Lighting();
	View = Camera();
	skybox = Object();
	return;
}


// Copy a world
World::World(const World& world)
{
	objects = world.objects;
	objects_front = world.objects_front;
	objects_2d = world.objects_2d;
	texts = world.texts;
	Light_Setup = world.Light_Setup;
	View = world.View;
	skybox = world.skybox;
	return;
}


// Deletes the world, opens memory
World::~World(void)
{
	return;
}


// Updates every object in the world, prepares to render again
void World::Update(const int time)
{
	if (Update_Function)
	{
		Update_Function(time);
	}
	for (std::size_t object = 0; object<objects.size(); ++object)
	{
		if (objects[object].Update)
		{
			objects[object].Update(time, objects[object]);
		}
	}
	for (std::size_t object = 0; object < objects_front.size(); ++object)
	{
		if (objects_front[object].Update)
		{
			objects_front[object].Update(time, objects_front[object]);
		}
	}
	for (std::size_t object = 0; object<objects_2d.size(); ++object)
	{
		if (objects_2d[object].Update)
		{
			objects_2d[object].Update(time, objects_2d[object]);
		}
	}
	return;
}


// Renders every object in the world
void World::Render(void) const
{

	// Setup Frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Indigo::Reshape();
	Light_Setup.Update_Lights();

	// Skbybox: Perspective, View Pointing, No View Translate, No Lighting, No Depth Test
	if (!skybox.Is_Blank)
	{
		View.Look_In_Place();
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		skybox.Render();
	}

	// Standard Object: View Transform, Perspective, Lighting, Depth Test
	View.Look();
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	for (std::size_t object = 0; object<objects.size(); ++object)
	{
		objects[object].Render();
	}

	// Front Object: View Pointing, Perspective, Lighting, Depth Test Cleared, No View Transform
	View.Look_Default();
	glClear(GL_DEPTH_BUFFER_BIT);
	for (std::size_t object = 0; object < objects_front.size(); ++object)
	{
		objects_front[object].Render();
	}

	// 2D Object / Text: No View Transform, Orthographic, No Lighting, No Depth Test
	Indigo::Reshape_2D();
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	for (std::size_t object = 0; object<objects_2d.size(); ++object)
	{
		objects_2d[object].Render();
	}
	for (std::size_t text = 0; text < texts.size(); ++text)
	{
		texts[text].Render();
	}
	
	// Finish Frame
	glutSwapBuffers();
	return;

}


/*
Add a skybox, a backdrop to the world. Accepts a filename of a texture.
The texture will be in the format of six square textures arranged like this:
     -----
     |Top|
-----------------------
|Left|Front|Right|Back|
-----------------------
    |Bottom|
    --------
Set to nullptr to remove the skybox.
*/
void World::Add_Skybox(const char * texture)
{

	if (!texture)
	{
		skybox = Object();
		return;
	}

	Mesh mesh = Mesh::Cube(0.5);
	mesh.Texture(texture);

	// Front
	mesh.Set_Texture_Coordinate(3 , Vertex(0.25, 0.3333));
	mesh.Set_Texture_Coordinate(2 , Vertex(0.50, 0.3333));
	mesh.Set_Texture_Coordinate(1 , Vertex(0.50, 0.6666));
	mesh.Set_Texture_Coordinate(0 , Vertex(0.25, 0.6666));
	// Back
	mesh.Set_Texture_Coordinate(6 , Vertex(0.75, 0.3333));
	mesh.Set_Texture_Coordinate(7 , Vertex(1.00, 0.3333));
	mesh.Set_Texture_Coordinate(4 , Vertex(1.00, 0.6666));
	mesh.Set_Texture_Coordinate(5 , Vertex(0.75, 0.6666));
	// Left
	mesh.Set_Texture_Coordinate(11, Vertex(0.00, 0.3333));
	mesh.Set_Texture_Coordinate(10, Vertex(0.25, 0.3333));
	mesh.Set_Texture_Coordinate(9 , Vertex(0.25, 0.6666));
	mesh.Set_Texture_Coordinate(8 , Vertex(0.00, 0.6666));
	// Right
	mesh.Set_Texture_Coordinate(14, Vertex(0.50, 0.3333));
	mesh.Set_Texture_Coordinate(15, Vertex(0.75, 0.3333));
	mesh.Set_Texture_Coordinate(12, Vertex(0.75, 0.6666));
	mesh.Set_Texture_Coordinate(13, Vertex(0.50, 0.6666));
	// Bottom
	mesh.Set_Texture_Coordinate(19, Vertex(0.25, 0.6666));
	mesh.Set_Texture_Coordinate(18, Vertex(0.50, 0.6666));
	mesh.Set_Texture_Coordinate(17, Vertex(0.50, 1.0000));
	mesh.Set_Texture_Coordinate(16, Vertex(0.25, 1.0000));
	// Top
	mesh.Set_Texture_Coordinate(21, Vertex(0.25, 0.0000));
	mesh.Set_Texture_Coordinate(20, Vertex(0.50, 0.0000));
	mesh.Set_Texture_Coordinate(23, Vertex(0.50, 0.3333));
	mesh.Set_Texture_Coordinate(22, Vertex(0.25, 0.3333));

	skybox = Object(0, 0, 0, mesh);
	return;

}


// Adds an object to the world to be rendered and updated, returns an object ID
int World::Add_Object(const Object& object)
{
	objects.push_back(object);
	int Object_ID = objects.size() - 1;
	objects[Object_ID].ID = Object_ID;
	return (Object_ID);
}

// Gets an object based on an index. DO NOT attempt to store the reference after a push_back. Ever.
Object& World::Get_Object(const int id) const
{
	return (const_cast <Object&>(objects[id]));
}

// Removes an object from the world based on an object ID
void World::Remove_Object(const int id)
{
	objects[id] = Object();
	return;
}

// Removes an object from the world based on object; gets ID automatically
void World::Remove_Object(const Object& object)
{
	objects[object.ID] = Object();
	return;
}


// Adds a front object to the world to be rendered and updated, returns an object ID.
// A front object is 3D, but doesn't get covered up by normal 3D objects. (Drawn in order of added)
// A front object is also not affected by the view. The camera is assumed to look in the negative Z direction at 0,0,0.
int World::Add_Front_Object(const Object& object)
{
	objects_front.push_back(object);
	int Object_ID = objects_front.size() - 1;
	objects_front[Object_ID].ID = Object_ID;
	return (Object_ID);
}

// Gets a front object based on an index. DO NOT attempt to store the reference after a push_back. Ever. Velociraptors.
Object& World::Get_Front_Object(const int id) const
{
	return (const_cast <Object&>(objects_front[id]));
}

// Removes a front object from the world based on an object ID
void World::Remove_Front_Object(const int id)
{
	objects_front[id] = Object();
	return;
}

// Removes a front object from the world based on object; gets ID automatically
void World::Remove_Front_Object(const Object& object)
{
	objects[object.ID] = Object();
	return;
}


// Adds an object to the world to be rendered and updated, returns an object ID
int World::Add_2D_Object(const Object& object)
{
	objects_2d.push_back(object);
	int Object_ID = objects_2d.size() - 1;
	objects_2d[Object_ID].ID = Object_ID;
	return (Object_ID);
}

// Gets an object based on an index. DO NOT attempt to store the reference after a push_back. Ever.
Object& World::Get_2D_Object(const int id) const
{
	return (const_cast <Object&>(objects_2d[id]));
}

// Removes an object from the world based on an object ID
void World::Remove_2D_Object(const int id)
{
	objects_2d[id] = Object();
	return;
}

// Removes an object from the world based on object; gets ID automatically
void World::Remove_2D_Object(const Object& object)
{
	objects_2d[object.ID] = Object();
	return;
}


// Add text to the world to be rendered, returns no handle to the text
int World::Add_Text(const Text& text)
{
	texts.push_back(text);
	int Object_ID = texts.size() - 1;
	texts[Object_ID].ID = Object_ID;
	return (Object_ID);
}

// Gets text based on an index. DO NOT attempt to store the reference after a push_back. Ever.
Text& World::Get_Text(const int id) const
{
	return (const_cast <Text&>(texts[id]));
}

// Removes text from the world based on an object ID
void World::Remove_Text(const int id)
{
	texts[id] = Text();
}

// Removes text from the world based on text; gets ID automatically
void World::Remove_Text(const Text& text)
{
	texts[text.ID] = Text();
}


// Add text to the world to be rendered, returns no handle to the text
int World::Add_Animation(const Animation& animation)
{
	animations.push_back(animation);
	int Object_ID = objects_2d.size() - 1;
	animations[Object_ID].ID = Object_ID;
	return (Object_ID);
}

// Removes text from the world based on an object ID
void World::Remove_Animation(const int id)
{
	animations[id] = Animation();
	return;
}

// Removes text from the world based on text; gets ID automatically
void World::Remove_Animation(const Animation& animation)
{
	animations[animation.ID] = Animation();
	return;
}


// Returns the number of objects in the world, simply objects.size()
int World::Number_Of_Objects(void)
{
	return (objects.size());
}


// Returns the number of 2d objects in the world, simply objects_2d.size()
int World::Number_Of_2D_Objects(void)
{
	return (objects_2d.size());
}


// Returns the number of texts in the world, simply texts.size()
int World::Number_Of_Texts(void)
{
	return (texts.size());
}

// Checks whether any object collides with another object, each collision testing returns -1 if no collision or object id for first
int World::Collide(const Object& object, const float add_x, const float add_y, const float add_z)
{
	for (std::size_t object = 0; object < objects.size(); ++object)
	{
		if (object != objects[object].ID && objects[object].World_Collide)
		{
			if (objects[object].Collide(object, add_x, add_y, add_z))
			{
				return (object);
				break;
			}
		}
	}
	return (-1);
}


// Checks whether any object will ever be intersected by a direction
int World::Collide(const Direction& position, const Direction& direction)
{
	for (std::size_t object = 0; object < objects.size(); ++object)
	{
		if (objects[object].World_Collide && objects[object].Collide(position, direction))
		{
			return (object);
		}
	}
	return (-1);
}


// Checks whether a vertex is within any object
int World::Collide(const Vertex& vertex, const float add_x, const float add_y, const float add_z)
{
	for (std::size_t object = 0; object < objects.size(); ++object)
	{
		if (objects[object].World_Collide && objects[object].Collide(vertex, add_x, add_y, add_z))
		{
			return (object);
		}
	}
	return (-1);
}