#pragma once

#include "GL/glew.h"

#include <vector>
#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include <map>
#include <string>


// Texture information that is sent to the GPU. Deals with memory allocation automatically
class Texture_Data
{
public:
	void Free(void);
	unsigned char * Image;
	int Width;
	int Height;
	int Channels;
	int Reduce_Filter;
	int Enlarge_Filter;
	int File_Hash;
};


// Just the data in the actual vertices of a mesh
struct Vertex_Data
{
	std::vector<glm::vec3> Positions;
	std::vector<glm::vec2> UVs;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec3> Bump_X_Normals;
	std::vector<glm::vec3> Bump_Y_Normals;
};


// All information that gets sent to the GPU (afterwards, it gets deleted)
struct Mesh_Data
{
	Vertex_Data Vertices;
	Texture_Data Texture;
	Texture_Data Bump_Map;
};


class Mesh
{

public:

	// Create a new, empty mesh
	Mesh(void);
	// Copy a mesh
	Mesh(const Mesh& mesh);
	// Assign a mesh. Resource management - Needs to be wibbled and yall
	Mesh& operator=(const Mesh& mesh);
	// Destroy the mesh. Free the GPU resources if it's the last one.
	~Mesh(void);
	// Create a new mesh by loading it from an obj file
	Mesh(const char * filename, const char * texture = nullptr, const char * bump_map = nullptr);
	// Create a new mesh by initializing it from cpu Vertex_Data (then you must texture it)
	Mesh(const Vertex_Data& initialize, const bool memory_optimize);

	// Specialized constructor for creating text
	static Mesh Text(const char * text, const float size = 0.1, const char * font = "Textures/Font.png", const glm::vec4& highlight = glm::vec4(0, 0, 0, 0), const int reduce_filter = GL_LINEAR_MIPMAP_LINEAR, const int enlarge_filter = GL_LINEAR);
	// Specialized constructor for creating 2D flat rectangles
	static Mesh Rectangle(const float width, const float height, const char * texture = nullptr, const char * bump_map = nullptr, const int reduce_filter = GL_LINEAR_MIPMAP_LINEAR, const int enlarge_filter = GL_LINEAR);
	// Returns CPU Mesh_Data of a Mesh from file
	static Vertex_Data File_Vertices(const char * filename);
	// Returns CPU Vertex_Data of a Text
	static Vertex_Data Text_Vertices(const char * text, const float size = 0.1);
	// Returns CPU Vertex_Data of a Rectangle
	static Vertex_Data Rectangle_Vertices(const float width, const float height);

	// The mesh is locked into place on the GPU
	void Initialize_Vertices(const Vertex_Data& initialize, const bool memory_optimize = true);

	// Update the hitbox knowing that this point exists in the mesh
	void Update_Hitbox(glm::vec3 vertex);

	// Load the texture and initialize it to the handle, if it's not a duplicate
	void Texture(const char * filename = nullptr, unsigned int * texture_handle = nullptr, const int reduce_filter = GL_LINEAR_MIPMAP_LINEAR, const int enlarge_filter = GL_LINEAR);
	// Simply calls Texture with texture_handle = &Bump_Texture_ID
	void Bump_Map(const char * filename = nullptr, const int reduce_filter = GL_LINEAR_MIPMAP_LINEAR, const int enlarge_filter = GL_LINEAR);
	// Initialize only the texture data, for animations which use the same mesh for example
	unsigned int Initialize_Texture(Texture_Data& initialize, unsigned int * texture_handle = nullptr);
	// Get the data from an image and return it
	static Texture_Data Load_Image(const char * filename = nullptr);

	// The actual data for each vertex is stored on the GPU.
	unsigned int Vertices_ID;
	// Each of these shorts points to a vertex, normal, uv set that is rendered.
	unsigned int Elements_ID;
	// The light normals on the GPU
	unsigned int Normals_ID;
	// The bump map alignments over X
	unsigned int Bump_X_Normals_ID;
	// The bump map alignments over Y
	unsigned int Bump_Y_Normals_ID;
	// The handle to the actual texture coordinates
	unsigned int UV_ID;
	// The actual texture image's handle number
	unsigned int Texture_ID;
	// The texture for the bump map
	unsigned int Bump_Texture_ID;

	// The size of the mesh In Elements
	unsigned int Size;
	// Hitbox used for collision, normally auto-generated. [0] = Least, [1] = Most.
	glm::vec3 Hitbox[2];

	// Stores this Mesh's hash in order to remove references when deleted
	unsigned int Obj_File_Hash;
	// Stores this Mesh's texture's hash
	unsigned int Texture_File_Hash;
	// Stores a hash of the filename mapped to the handle, so that the same file isn't loaded twice
	static std::map<unsigned int, std::vector<unsigned int>> Load_Once;

	// Reference counting so we can destroy elements when we're done
	unsigned short * Vertex_References;
	// Reference counting so we can destroy textures when we're done
	unsigned short * Texture_References;

private:

};
