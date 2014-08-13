// Allows the world to be viewed from different points

#pragma once

#include "Object.h"
#include "Direction.h"

#include "glm/glm.hpp"
#include "glm/mat4x4.hpp"


class Camera
{

public:

	// Create a new camera by a position and a direction
	Camera(const float x = 0.0, const float y = 0.0, const float z = 0.0,
		const Direction looking = Direction(1.0), const float near = 0.2, const float far = 1000.0,
		const Direction above = Direction(1.0, 0.0, 90.0), const int field_of_view_x = 90);
	// Create a camera based off another
	Camera(const Camera& camera);
	// Remove the camera and free memory
	~Camera(void);

	// Place the camera at a point
	void Place(const float x, const float y, const float z);
	// Move relative to the way it's pointing and where it is
	void Move(const float foward, const float side = 0.0, const float up = 0.0);
	// Look in a direction
	void Look_At(const Direction& direction);
	// Look at a point
	void Look_At(const float x, const float y, const float z);
	// Look in a direction relative to the camera
	void Look_Towards(const Direction& direction);
	// Look at a point relative to the camera
	void Look_Towards(const float x, const float y, const float z);
	// Look at an object, should be updated frame-ly to maintain accuracy
	void Watch(const Object& object, const Direction& direction);

	// Set up the projection matrix for this frame
	glm::mat4 Project(void) const;
	// Set up the projection matrix for this frame
	glm::mat4 Project_2D(void) const;

	// Look through the camera for this frame with a full transformation, actually executed by [I200: World, I300: Fragment Shader]
	glm::mat4 Look(void) const;
	// Look through the camera, but at the position 0,0,0. Used for Skybox.
	glm::mat4 Look_In_Place(void) const;

	// Generates a target for rendering other than the screen for render to texture (for shadows, gfx, etc)
	// If resolution is 0,0 it will be treated as Indigo::Screen_Width, Indigo::Screen_Height
	// Returns handle to texture
	// If we don't succeed, Render_Target is reset to 0 (screen) and texture handle is 0
	unsigned int Generate_Render_Target(const glm::vec2& resolution = glm::vec2(0, 0), const bool is_depth_map = false, const bool is_3d = true);
	// Deletes a render target that was added id'th after the first. 0 to delete screen render, 1 to delete a Generated one, etc
	void Delete_Render_Target(const int id = 0);
	// Resets render target to the screen (Render Target = 0)
	void Screen_Render(void);
	// Compiles the vertex and fragment shaders to use for completing the rendering
	// Uses many files from Indigo/Shaders/; recommend copying whole Shaders directory into Indigo/ in release directory
	void Shader(const char * vertex, const char * fragment);
	// Gets the location of a shader argument. Must be used for uploading data
	int Shader_Location(const char * name, const bool uniform = false);

	// The direction of the camera that is up.
	Direction Up;
	// The camera's position
	float X;
	float Y;
	float Z;
	// Data can't be closer than this
	float Near;
	// Data can't go farther than this
	float Far;
	// The way the camera is looking
	Direction Eye;
	// The angle of the field of view on the x axis
	int Field_Of_View;

	// The render resolution for the camera. If (0,0) it's (Indigo::Screen_Width, Indigo::Screen_Height)
	std::vector<glm::vec2> Resolutions;
	// The render target: 0 is the screen
	std::vector<unsigned int> Render_Targets;
	// The handle for the shader attached to this camera, defaults to Indigo/Shaders/Default.vs, and .fs
	unsigned int Shader_Index;

	// Stores a hash of the filename mapped to each shader so that the same shader isn't loaded twice (basically Mesh.Load_Once)
	static std::map<unsigned int, unsigned int> Load_Once;

private:

};