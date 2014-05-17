// Defines a class for holding objects

#include "GL/glew.h"

#include "World.h"

#include "Animation.h"
#include "Indigo.h"

#include <fstream>
#include <string>
#include <iostream>


// Create a new, empty world
World::World(void)
{
	objects = std::vector<Object>();
	objects_front = std::vector<Object>();
	objects_2d = std::vector<Object>();
	animations = std::vector<Animation>();
	skybox = Object();
	Update_Function = nullptr;
	Light_Setup = Lighting();
	View = Camera();
	Matrix_Handle = 0;
	View_Matrix = 0;
	Model_Matrix = 0;
	Shader_Index = 0;
	return;
}


// Copy a world
World::World(const World& world)
{
	objects = world.objects;
	objects_front = world.objects_front;
	objects_2d = world.objects_2d;
	animations = world.animations;
	skybox = world.skybox;
	Update_Function = world.Update_Function;
	Light_Setup = world.Light_Setup;
	View = world.View;
	Matrix_Handle = world.Matrix_Handle;
	View_Matrix = world.View_Matrix;
	Model_Matrix = world.Model_Matrix;
	Shader_Index = world.Shader_Index;
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
	for (std::size_t object = 0; object<objects.size(); ++object)
	{
		objects[object].Update(time);
	}
	for (std::size_t object = 0; object < objects_front.size(); ++object)
	{
		objects_front[object].Update(time);
	}
	for (std::size_t object = 0; object<objects_2d.size(); ++object)
	{
		objects_2d[object].Update(time);
	}
	return;
}


// Renders every object in the world
void World::Render(void) const
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(Shader_Index);

	glm::mat4 project = View.Project();
	glm::mat4 project_2d = View.Project_2D();
	glm::mat4 view = View.Look();

	Indigo::Current_World.Light_Setup.Update_Lights();

	// Skbybox: Perspective, View Pointing, No View Translate, No Lighting, No Depth Test
	if (skybox.Data.Size != 0)
	{
		skybox.Render(project, View.Look_In_Place());
	}

	// Standard Object: View Transform, Perspective, Lighting, Depth Test
	glEnable(GL_DEPTH_TEST);
	for (std::size_t object = 0; object<objects.size(); ++object)
	{
		objects[object].Render(project, view);
	}

	// Front Object: View Pointing, Perspective, Lighting, Depth Test Cleared, No View Transform
	glClear(GL_DEPTH_BUFFER_BIT);
	for (std::size_t object = 0; object < objects_front.size(); ++object)
	{
		objects_front[object].Render(project, glm::mat4(1));
	}

	// 2D Object / Text: No View Transform, Orthographic, No Lighting, No Depth Test
	glDisable(GL_DEPTH_TEST);
	for (std::size_t object = 0; object<objects_2d.size(); ++object)
	{
		objects_2d[object].Render(project_2d, glm::mat4(1), false);
	}

	return;

}


// Compiles and puts in place custom Vertex and Fragment Shaders
void World::Shader(const char * vertex, const char * fragment)
{

	// Vertex
	std::ifstream vs_stream = std::ifstream(vertex, std::ios::in);
	if (!vs_stream)
	{
		std::cout << "Uhh... Huh? Couldn't find vertex shader \"" << vertex << "\"! Failing silently." << std::endl;
		return;
	}
	std::string total;
	std::string line;
	while (std::getline(vs_stream, line))
	{
		total += line;
		total += "\n";
	}
	vs_stream.close();
	const char * source = total.c_str();
	unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &source, NULL);
	glCompileShader(vertex_shader);
	int succeeded;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &succeeded);
	if (succeeded == GL_FALSE)
	{
		int size;
		glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &size);
		char * data = new char[size + 1];
		glGetShaderInfoLog(vertex_shader, size, NULL, data);
		std::cout << "KITTEN KILLER! Failing silently. In vertex shader:" << std::endl << data << std::endl;
		delete[] data;
		return;
	}

	// Fragment
	std::ifstream fs_stream = std::ifstream(fragment, std::ios::in);
	if (!fs_stream)
	{
		std::cout << "Uhh... Huh? Couldn't find fragment shader \"" << fragment << "\"! Failing silently." << std::endl;
		return;
	}
	total = std::string();
	while (std::getline(fs_stream, line))
	{
		total += line;
		total += "\n";
	}
	fs_stream.close();
	const char * fragment_source = total.c_str();
	unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_source, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &succeeded);
	if (succeeded == GL_FALSE)
	{
		int size;
		glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &size);
		char * data = new char[size + 1];
		glGetShaderInfoLog(fragment_shader, size, NULL, data);
		std::cout << "KITTEN KILLER! Failing silently. In vertex shader:" << std::endl << data << std::endl;
		delete [] data;
		return;
	}

	// Link
	Shader_Index = glCreateProgram();
	glAttachShader(Shader_Index, vertex_shader);
	glAttachShader(Shader_Index, fragment_shader);
	glLinkProgram(Shader_Index);
	glGetProgramiv(Shader_Index, GL_LINK_STATUS, &succeeded);
	if (succeeded == GL_FALSE)
	{
		int size;
		glGetProgramiv(Shader_Index, GL_INFO_LOG_LENGTH, &size);
		char * data = new char[size];
		glGetProgramInfoLog(Shader_Index, size, NULL, data);
		std::cout << "KITTEN KILLER! Actually, linker, so probably just GLSL being a nerd. Failing silently, but: " << std::endl << data << std::endl;
		delete[] data;
		glDeleteShader(vertex_shader);
		return;
	}

	// Cleanup
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return;

}


// Get the location of a variable in a shader
int World::Shader_Location(const char * name, const bool uniform)
{
	if (uniform)
	{
		return glGetUniformLocation(Shader_Index, name);
	}
	else
	{
		return glGetAttribLocation(Shader_Index, name);
	}
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


// Returns the number of objects in the world, simply objects.size()
int World::Number_Of_Objects(void) const
{
	return (objects.size());
}


// Returns the number of 2d objects in the world, simply objects_2d.size()
int World::Number_Of_2D_Objects(void) const
{
	return (objects_2d.size());
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