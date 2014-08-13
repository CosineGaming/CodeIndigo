// Allows the world to be viewed from different points

#include "Camera.h"
#include "Indigo.h"

#include <fstream>
#include <string>
#include <stdlib.h>
#include <iostream>
#include "GLFW/glfw3.h"
#include "glm/vec3.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"



// Create a new camera by a position and a direction
Camera::Camera(const float x, const float y, const float z, const Direction looking, const float near, const float far, const Direction above, const int field_of_view_x) :
	X(x),
	Y(y),
	Z(z),
	Eye(looking),
	Near(near),
	Far(far),
	Up(above),
	Field_Of_View(field_of_view_x),
	Resolutions(std::vector<glm::vec2>(1, glm::vec2(0,0))),
	Render_Targets(std::vector<unsigned int>(1, 0)),
	Shader_Index(0)
{
	return;
}


// Create a camera based off another
Camera::Camera(const Camera& camera) :
	X(camera.X),
	Y(camera.Y),
	Z(camera.Z),
	Near(camera.Near),
	Far(camera.Far),
	Up(camera.Up),
	Field_Of_View(camera.Field_Of_View),
	Resolutions(camera.Resolutions),
	Render_Targets(camera.Render_Targets),
	Shader_Index(camera.Shader_Index)
{
	Place(camera.X, camera.Y, camera.Z);
	Look_At(camera.Eye);
	return;
}



// Remove the camera and free memory
Camera::~Camera(void)
{
	return;
}


// Place the camera at a point
void Camera::Place(const float x, const float y, const float z)
{
	X = x;
	Y = y;
	Z = z;
	return;
}


// Move relative to the way it's pointing and where it is
void Camera::Move(const float forward, const float side, const float up)
{
	Direction direction = Eye;
	direction.Set_Direction(forward, direction.Get_X_Angle(), 0.0);
	X += direction.Get_X();
	Y += direction.Get_Y();
	Z += direction.Get_Z();
	direction.Set_Direction(side, direction.Get_X_Angle() + 90.0, 0.0);
	X += direction.Get_X();
	Y += direction.Get_Y();
	Z += direction.Get_Z();
	direction.Set_Direction(up, 0.0, 90.0);
	X += direction.Get_X();
	Y += direction.Get_Y();
	Z += direction.Get_Z();
	return;
}


// Look in a direction
void Camera::Look_At(const Direction& direction)
{
	Eye = direction.Distance(Direction::Coordinates(X, Y, Z));
	return;
}


// Look at a point
void Camera::Look_At(const float x, const float y, const float z)
{
	Eye.Set_Coordinates(x - X, y - Y, z - Z);
	return;
}


// Look in a direction
void Camera::Look_Towards(const Direction& direction)
{
	Eye = direction;
	return;
}


// Look at a point
void Camera::Look_Towards(const float x, const float y, const float z)
{
	Eye.Set_Coordinates(x, y, z);
	return;
}


// Look at an object, should be updated frame-ly to maintain accuracy
void Camera::Watch(const Object& object, const Direction& relative_camera_position)
{
	Direction position = object.Facing;
	position.Add_Direction(0.0, relative_camera_position.Get_X_Angle(), relative_camera_position.Get_Y_Angle());
	Place(position.Get_X() + object.X,
		position.Get_Y() + object.Y,
		position.Get_Z() + object.Z);
	Eye.Set_Coordinates(object.X - X, object.Y - Y, object.Z - Z);
	return;
}


// Set up the projection matrix for this frame
glm::mat4 Camera::Project(void) const
{
	return glm::perspective(Field_Of_View / Indigo::Aspect_Ratio, Indigo::Aspect_Ratio, Near, Far);
}


// Set up the 2D orthogonal matrix for this frame
glm::mat4 Camera::Project_2D(void) const
{
	return glm::ortho(-1 * Indigo::Aspect_Ratio, Indigo::Aspect_Ratio, -1.0f, 1.0f, -1.0f, 1.0f);
}


// Look through the camera for this frame with a full transformation
glm::mat4 Camera::Look(void) const
{
	return glm::lookAt(glm::vec3(X, Y, Z), glm::vec3(X + Eye.Get_X(), Y + Eye.Get_Y(), Z + Eye.Get_Z()),
		glm::vec3(Up.Get_X(), Up.Get_Y(), Up.Get_Z()));
}


// Look through the camera, but at the position 0,0,0. Used for Skybox.
glm::mat4 Camera::Look_In_Place(void) const
{
	return glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(Eye.Get_X(), Eye.Get_Y(), Eye.Get_Z()),
		glm::vec3(Up.Get_X(), Up.Get_Y(), Up.Get_Z()));
}


// Generates a target for rendering other than the screen for render to texture (for shadows, gfx, etc)
// If resolution is 0,0 it will be Indigo::Screen_Width, Indigo::Screen_Height
// If we don't succeed, Render_Target is reset to 0 (screen)
unsigned int Camera::Generate_Render_Target(const glm::vec2& resolution, const bool is_depth_map, const bool is_3d)
{
	glm::vec2 res;
	if (resolution.x != 0 && resolution.y != 0)
	{
		Resolutions.push_back(resolution);
		res = resolution;
	}
	else
	{
		Resolutions.push_back(glm::vec2(0, 0));
		res = glm::vec2(Indigo::Screen_Width, Indigo::Screen_Height);
	}
	Render_Targets.push_back(0);
	int target = Render_Targets.size() - 1;
	glGenFramebuffers(1, &Render_Targets[target]);
	glBindFramebuffer(GL_FRAMEBUFFER, Render_Targets[target]);
	unsigned int output;
	glGenTextures(1, &output);
	glBindTexture(GL_TEXTURE_2D, output);
	glTexImage2D(GL_TEXTURE_2D, 0, is_depth_map ? GL_DEPTH_COMPONENT16 : GL_RGB, res.x, res.y, 0, is_depth_map ? GL_DEPTH_COMPONENT : GL_RGB, is_depth_map ? GL_FLOAT : GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (is_3d && !is_depth_map)
	{
		unsigned int depth_test;
		glGenRenderbuffers(1, &depth_test);
		glBindRenderbuffer(GL_RENDERBUFFER, depth_test);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, res.x, res.y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_test);
	}
	glFramebufferTexture(GL_FRAMEBUFFER, is_depth_map ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0, output, 0);
	if (is_depth_map)
	{
		glDrawBuffer(GL_NONE);
	}
	else
	{
		unsigned int buffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, buffers);
	}
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Screen_Render();
		std::cout << "Failed to add render target. Failing silently." << std::endl;
		return 0;
	}
	return output;
}


// Deletes a render target that was added id'th after the first. 0 to delete screen render, 1 to delete a Generated one, etc
void Camera::Delete_Render_Target(const int id)
{
	Resolutions.erase(Resolutions.begin() + id);
	Render_Targets.erase(Render_Targets.begin() + id);
}


// Resets render target to the screen (Render Target = 0)
void Camera::Screen_Render(void)
{
	Resolutions = std::vector<glm::vec2>(1, glm::vec2(0,0));
	Render_Targets = std::vector<unsigned int>(1, 0);
}


// Compiles the vertex and fragment shaders to use for completing the rendering
// Uses many files from Indigo/Shaders/; recommend copying whole Shaders directory into Indigo/ in release directory
void Camera::Shader(const char * vertex, const char * fragment)
{

	unsigned int hash = 0;
	for (int i = 0; vertex[i] != '\0'; ++i)
	{
		hash += vertex[i];
	}
	for (int i = 0; fragment[i] != '\0'; ++i)
	{
		hash += fragment[i];
	}
	hash %= Hash_Modulo;

	std::map<unsigned int, unsigned int>::iterator location = Load_Once.find(hash);

	if (location != Load_Once.end())
	{
		Shader_Index = location->second;
	}
	else
	{

		// Vertex
		std::ifstream vs_func_stream = std::ifstream("Indigo/Shaders/Library.vs");
		if (!vs_func_stream)
		{
			std::cout << "Uhh... Huh? Couldn't find default vertex functions file \"Indigo/Shaders/Library.vs\"! Failing silently." << std::endl;
			return;
		}
		std::string total;
		std::string line;
		while (std::getline(vs_func_stream, line))
		{
			total += line;
			total += "\n";
		}
		vs_func_stream.close();
		std::ifstream vs_stream = std::ifstream(vertex, std::ios::in);
		if (!vs_stream)
		{
			std::cout << "Uhh... Huh? Couldn't find vertex shader \"" << vertex << "\"! Failing silently." << std::endl;
			return;
		}
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
			delete [] data;
			return;
		}

		// Fragment
		std::ifstream fs_func_stream = std::ifstream("Indigo/Shaders/Library.fs", std::ios::in);
		if (!fs_func_stream)
		{
			std::cout << "Uhh... Huh? Couldn't find default fragment functions file \"Indigo/Shaders/Library.fs\"! Failing silently." << std::endl;
			return;
		}
		total = std::string();
		while (std::getline(fs_func_stream, line))
		{
			total += line;
			total += "\n";
		}
		fs_func_stream.close();
		std::ifstream fs_stream = std::ifstream(fragment, std::ios::in);
		if (!fs_stream)
		{
			std::cout << "Uhh... Huh? Couldn't find fragment shader \"" << fragment << "\"! Failing silently." << std::endl;
			return;
		}
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
			std::cout << "KITTEN KILLER! Failing silently. In fragment shader:" << std::endl << data << std::endl;
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
			std::cout << "KITTEN KILLER! Actually, linker, so probably just GLSL being stupid. Failing silently, but: " << std::endl << data << std::endl;
			delete [] data;
			glDeleteShader(vertex_shader);
			return;
		}

		// Cleanup
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		Load_Once[hash] = Shader_Index;

	}

	return;

}


// Gets the location of a shader argument. Must be used for uploading data
int Camera::Shader_Location(const char * name, const bool uniform)
{
	if (Shader_Index == 0)
	{
		// To avoid the common pitfall of forgetting to assign shaders; a "default argument" of sorts
		Shader("Indigo/Shaders/Default.vs", "Indigo/Shaders/Default.fs");
		glUseProgram(Shader_Index);
	}
	if (uniform)
	{
		return glGetUniformLocation(Shader_Index, name);
	}
	else
	{
		return glGetAttribLocation(Shader_Index, name);
	}
}


// Stores a hash of the filename mapped to each shader so that the same shader isn't loaded twice (basically Mesh.Load_Once)
std::map<unsigned int, unsigned int> Camera::Load_Once = std::map<unsigned int, unsigned int>();
