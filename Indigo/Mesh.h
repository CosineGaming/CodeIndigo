#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include <map>
#include <string>


class Mesh
{

public:

	// Create a new, empty mesh
	Mesh(void);
	// Copy a mesh
	Mesh(const Mesh& mesh);
	// Assign a mesh. Resource management -- Needs to be wibbled and yall
	Mesh& operator=(const Mesh& mesh);
	// Destroy the mesh. Free the GPU resources if it's the last one.
	~Mesh(void);
	// Create a new mesh by loading it from an obj file
	Mesh(const char * filename, const char * texture = nullptr);
	// Specialized constructor for creating text
	static Mesh Text(const char * text, const float size = 0.1, const char * font = "Textures/Font.png", const glm::vec4& highlight = glm::vec4(1, 1, 1, 0));

	// The mesh is locked into place on the GPU
	void Initialize(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& uvs, const std::vector<glm::vec3>& normals,
		const std::vector<glm::vec3>& bump_x_normals, const std::vector<glm::vec3>& bump_y_normals, const bool memory_optimize = true);

	// Update the hitbox knowing that this point exists in the mesh
	void Update_Hitbox(glm::vec3 vertex);
	// Actually push the texture to the GPU.
	void Initialize_Texture(unsigned int& handle, const unsigned char * data, const int width, const int height, const int channels);
	// Texture the entire mesh with one file. PNGs Costly.
	void Texture(const char * filename = nullptr, glm::vec3 background = glm::vec3(-1, -1, -1));
	// A special texture that adds normals to each pixel. Default blue. PNGs Costly.
	void Bump_Map(const char * filename = nullptr);

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
	// Hitbox used for collision, normally auto-generated. Radius of shape around center, rotation dependent
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