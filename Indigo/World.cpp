// Defines a class for holding objects

#include "GL/glew.h"

#include "World.h"

#include "Animation.h"
#include "Indigo.h"

#include <iostream>


// Create a new, empty world
World::World(void) :
	objects(std::vector<Object>()),
	objects_front(std::vector<Object>()),
	objects_2d(std::vector<Object>()),
	animations(std::vector<Animation>()),
	skybox(Object()),
	Update_Function(nullptr),
	Pre_Render_Function(nullptr),
	Post_Render_Function(nullptr),
	Views(std::vector<Camera>()),
	Light_Setup(Lighting()),
	Matrix_Handle(0),
	View_Matrix(0),
	Model_Matrix(0)
{
	Views.push_back(Camera());
	return;
}


// Copy a world
World::World(const World& world) :
	objects(world.objects),
	objects_front(world.objects_front),
	objects_2d(world.objects_2d),
	animations(world.animations),
	skybox(world.skybox),
	Update_Function(world.Update_Function),
	Pre_Render_Function(world.Pre_Render_Function),
	Post_Render_Function(world.Post_Render_Function),
	Light_Setup(world.Light_Setup),
	Views(world.Views),
	Matrix_Handle(world.Matrix_Handle),
	View_Matrix(world.View_Matrix),
	Model_Matrix(world.Model_Matrix)
{
	return;
}


// Deletes the world, opens memory
World::~World(void)
{
	return;
}


// Updates every object in the world, prepares to render again
void World::Update(const float time)
{
	if (Update_Function)
	{
		Update_Function(time);
	}
	for (std::size_t object = 0; object < objects.size(); ++object)
	{
		objects[object].Update(time);
	}
	for (std::size_t object = 0; object < objects_front.size(); ++object)
	{
		objects_front[object].Update(time);
	}
	for (std::size_t object = 0; object < objects_2d.size(); ++object)
	{
		objects_2d[object].Update(time);
	}
	return;
}


// Renders every object in the world
void World::Render(void)
{

	// These loops are in reverse order so that the screen is drawn last

	for (int render = Views.size() - 1; render >= 0; --render)
	{

		for (int target = Views[render].Render_Targets.size() - 1; target >= 0; --target)
		{

			// Though this looks like a complex and long-lasting for loop, for standard rendering this will only run once

			glBindFramebuffer(GL_FRAMEBUFFER, Views[render].Render_Targets[target]);
			glViewport(0, 0, Views[render].Resolutions[target].x == 0 ? Indigo::Screen_Width : Views[render].Resolutions[target].x, Views[render].Resolutions[target].y == 0 ? Indigo::Screen_Height : Views[render].Resolutions[target].y);

			glUseProgram(Views[render].Shader_Index);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 project = Views[render].Project();
			glm::mat4 project_2d = Views[render].Project_2D();
			glm::mat4 view = Views[render].Look();

			Light_Setup.Update_Lights(Views[render], view);

			// Skbybox: Perspective, View Pointing, No View Translate, No Lighting, No Depth Test
			if (skybox.Data.Size != 0)
			{
				skybox.Render(Views[render], project, Views[render].Look_In_Place());
			}

			// Standard Object: View Transform, Perspective, Lighting, Depth Test
			glEnable(GL_DEPTH_TEST);
			for (std::size_t object = 0; object < objects.size(); ++object)
			{
				objects[object].Render(Views[render], project, view);
			}

			// Front Object: View Pointing, Perspective, Lighting, Depth Test Cleared, No View Transform
			glClear(GL_DEPTH_BUFFER_BIT);
			for (std::size_t object = 0; object < objects_front.size(); ++object)
			{
				objects_front[object].Render(Views[render], project, glm::mat4(1));
			}

			// 2D Object / Text: No View Transform, Orthographic, No Lighting, No Depth Test
			glDisable(GL_DEPTH_TEST);
			for (std::size_t object = 0; object < objects_2d.size(); ++object)
			{
				objects_2d[object].Render(Views[render], project_2d, glm::mat4(1), false);
			}

		}

	}

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

	/*if (!texture)
	{
		skybox = Object();
		return;
	}

	Mesh mesh = Mesh::Cube(0.5);
	mesh.Texture(texture);

	// Front
	mesh.Set_Texture_Coordinate(3 , glm::vec2(0.25, 0.3333));
	mesh.Set_Texture_Coordinate(2 , glm::vec2(0.50, 0.3333));
	mesh.Set_Texture_Coordinate(1 , glm::vec2(0.50, 0.6666));
	mesh.Set_Texture_Coordinate(0 , glm::vec2(0.25, 0.6666));
	// Back
	mesh.Set_Texture_Coordinate(6 , glm::vec2(0.75, 0.3333));
	mesh.Set_Texture_Coordinate(7 , glm::vec2(1.00, 0.3333));
	mesh.Set_Texture_Coordinate(4 , glm::vec2(1.00, 0.6666));
	mesh.Set_Texture_Coordinate(5 , glm::vec2(0.75, 0.6666));
	// Left
	mesh.Set_Texture_Coordinate(11, glm::vec2(0.00, 0.3333));
	mesh.Set_Texture_Coordinate(10, glm::vec2(0.25, 0.3333));
	mesh.Set_Texture_Coordinate(9 , glm::vec2(0.25, 0.6666));
	mesh.Set_Texture_Coordinate(8 , glm::vec2(0.00, 0.6666));
	// Right
	mesh.Set_Texture_Coordinate(14, glm::vec2(0.50, 0.3333));
	mesh.Set_Texture_Coordinate(15, glm::vec2(0.75, 0.3333));
	mesh.Set_Texture_Coordinate(12, glm::vec2(0.75, 0.6666));
	mesh.Set_Texture_Coordinate(13, glm::vec2(0.50, 0.6666));
	// Bottom
	mesh.Set_Texture_Coordinate(19, glm::vec2(0.25, 0.6666));
	mesh.Set_Texture_Coordinate(18, glm::vec2(0.50, 0.6666));
	mesh.Set_Texture_Coordinate(17, glm::vec2(0.50, 1.0000));
	mesh.Set_Texture_Coordinate(16, glm::vec2(0.25, 1.0000));
	// Top
	mesh.Set_Texture_Coordinate(21, glm::vec2(0.25, 0.0000));
	mesh.Set_Texture_Coordinate(20, glm::vec2(0.50, 0.0000));
	mesh.Set_Texture_Coordinate(23, glm::vec2(0.50, 0.3333));
	mesh.Set_Texture_Coordinate(22, glm::vec2(0.25, 0.3333));

	skybox = Object(0, 0, 0, mesh);*/
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


// Returns the number of objects in the world, simply objects.size()
int World::Number_Of_Objects(void) const
{
	return (objects.size());
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

// Returns the number of texts in the world, simply objects_front.size()
int World::Number_Of_Front_Objects(void) const
{
	return (objects_front.size());
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

// Returns the number of 2d objects in the world, simply objects_2d.size()
int World::Number_Of_2D_Objects(void) const
{
	return (objects_2d.size());
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

// Returns the number of texts in the world, simply animations.size()
int World::Number_Of_Animations(void) const
{
	return (animations.size());
}


// Checks whether any object collides with another object, each collision testing returns -1 if no collision or object id for first
int World::Collide(const Object& with, const float add_x, const float add_y, const float add_z) const
{
	for (std::size_t object = 0; object < objects.size(); ++object)
	{
		if (object != with.ID && objects[object].World_Collide)
		{
			if (with.Collide(objects[object], add_x, add_y, add_z))
			{
				return (object);
				break;
			}
		}
	}
	return (-1);
}


// Checks whether any object will ever be intersected by a direction
int World::Collide(const glm::vec2& position, const glm::mat4& camera_position) const
{
	float least = 2;
	int answer = -1;
	for (std::size_t object = 0; object < objects.size(); ++object)
	{
		if (objects[object].World_Collide)
		{
			float result = objects[object].Collide(position, camera_position);
			if (result < least)
			{
				least = result;
				answer = object;
			}
		}
	}
	return (answer);
}


// Checks whether a vertex is within any object
int World::Collide(const glm::vec3& vertex, const float add_x, const float add_y, const float add_z) const
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
