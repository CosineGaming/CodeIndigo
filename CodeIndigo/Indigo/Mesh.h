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
	void Initialize(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& uvs, const std::vector<glm::vec3>& normals);

	// Update the hitbox knowing that this point exists in the mesh
	void Update_Hitbox(glm::vec3 vertex);
	// Texture the entire mesh with one file
	void Texture(const char * filename=nullptr);

	// Hitbox used for collision, normally auto-generated. Radius of shape around center, rotation dependent
	glm::vec3 Hitbox[2];

	// The color for the whole mesh, not one per vertex. RGB. (No alpha. If you want that, use Textures.)
	glm::vec4 Color;
	// The shininess of the mesh, actually the size of specular highlights. Higher is smaller highlights is shinier
	float Shine;

	// The actual data for each vertex is stored on the GPU.
	unsigned int Vertices_ID;
	// Each of these shorts points to a vertex, normal, uv set that is rendered.
	unsigned int Elements_ID;
	// The light normals on the GPU
	unsigned int Normals_ID;
	// The handle to the actual texture coordinates
	unsigned int UV_ID;
	// The actual texture image's handle number
	unsigned int Texture_ID;

	// The size of the mesh In Elements
	unsigned int Size;

private:

};