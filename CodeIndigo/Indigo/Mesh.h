#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"


class Mesh
{

public:

	// Create a new, empty mesh
	Mesh(void);
	// Copy a mesh
	Mesh(const Mesh& mesh);
	// Create a new mesh by loading it from an obj file
	Mesh(const char * filename, const char * texture = nullptr, const float shine = 60.0, const glm::vec4& color = glm::vec4(1, 1, 1, 1));
	// Specialized constructor for creating text
	static Mesh Text(const char * text, const float size = 0.1, const char * font = "Textures/Font.bmp", const glm::vec4& color = glm::vec4(1, 1, 1, 1));
	// Destroy the mesh
	~Mesh(void);

	// The mesh is locked into place on the GPU
	void Initialize(const std::vector<glm::vec3>& temp_vertices, const std::vector<glm::vec2>& temp_texture_coordinates, const std::vector<glm::vec3>& temp_normals);

	// Calculate one flat normal. Assumes index is end of group.
	glm::vec3 Find_Flat_Normal(const glm::vec3& one, const glm::vec3& two, const glm::vec3& three) const;
	// Update the hitbox knowing that this point exists in the mesh
	void Update_Hitbox(glm::vec3 vertex);
	// Texture the entire mesh with one file
	void Texture(const char * filename=nullptr);
	// Get the coordinates of the texture, as a vertex with X and Y (and Z omitted) for a vertex
	glm::vec2 Auto_Texture_Coordinate(const int index) const;

	// Get the number of Vertices in the mesh
	int Size(void) const;

	// Hitbox used for collision, normally auto-generated. Radius of shape around center, rotation independent
	glm::vec3 Hitbox[2];

	// The color for the whole mesh, not one per vertex. RGB. (No alpha. If you want that, use Textures.)
	glm::vec4 Color;
	// The shininess of the mesh, actually the size of specular highlights. Higher is smaller highlights is shinier
	float Shine;

	// The actual data is stored on the GPU. Here's the index for gathering it.
	unsigned int Vertices_ID;
	// The list of verts to use is also on the GPU. Here you yoga.
	unsigned int Elements_ID;
	// The light normals on the GPU
	unsigned int Normals_ID;
	// The texture handle number
	unsigned int Texture_ID;
	// The handle to the actual texture coordinates
	unsigned int UV_ID;

private:

	// Once the elements are initialized, we only need to remember the number of them
	unsigned int number_elements;

};