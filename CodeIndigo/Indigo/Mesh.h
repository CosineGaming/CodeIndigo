#pragma once

#include <vector>
#include "glm/glm.hpp"


class Mesh
{

public:

	// Create a new, empty mesh
	Mesh(void);
	// Copy a mesh
	Mesh(const Mesh& mesh);
	// Create a new mesh by loading it from an obj file
	Mesh(const char * filename, const char * texture=nullptr, const float shine = 60.0, const glm::vec3& color = glm::vec3(1,1,1));
	// Destroy the mesh
	~Mesh(void);

	// Once added to the object, the mesh is locked into place (on the GPU)
	void Initialize(void);

	// Allows [] to get a vertex like an array
	const glm::vec3& operator[](const int index) const;
	// Allows += to add a mesh to the end
	Mesh& operator+=(const Mesh& mesh);
	// Allows += to add a vector of vertices to the end
	Mesh& operator+=(const std::vector <glm::vec3>& vertices);
	// Allows + to add a vertex to the end
	Mesh operator+(const glm::vec3& vertex) const;
	// Allows + to add a mesh to the end
	Mesh operator+(const Mesh& mesh) const;

	// Gets a vertex by its index
	const glm::vec3& Get_Vertex(const int index) const;
	// Gets a vertex that can be modified
	glm::vec3& Edit_Vertex(const int index);

	// Calculate one flat normal. Assumes index is end of group.
	glm::vec3 Find_Flat_Normal(const int index) const;
	// Get the normal for a specific vertex
	glm::vec3 Flat_Normal(const int index) const;
	// Get the smoother per-vertex normal for a vertex; calculate if needed
	glm::vec3 Smooth_Normal(const int index) const;
	// Update the hitbox knowing that this point exists in the mesh
	void Update_Hitbox(glm::vec3 vertex);
	// Texture the entire mesh with one file
	void Texture(const char * filename);
	// Get the coordinates of the texture, as a vertex with X and Y (and Z omitted) for a vertex
	glm::vec2 Auto_Texture_Coordinate(const int index) const;

	// Get the number of Vertices in the mesh
	int Size(void) const;
	// Number of actual different vertices defined
	int Vertex_Data_Amount(void) const;

	// Hitbox used for collision, normally auto-generated. Radius of shape around center, rotation independent
	glm::vec3 Hitbox[2];
	// Add points to the mesh in function notation
	void Add(const glm::vec3& vertex);
	void Add(const Mesh& mesh);
	void Add(const std::vector <glm::vec3>& vertices);

	// The color for the whole mesh, not one per vertex. RGB. (No alpha. If you want that, use Textures.)
	glm::vec3 Color;
	// The shininess of the mesh, actually the size of specular highlights (higher is smaller)
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

	// The vertices pool to choose from. Never rendered unless in elements! (x,y,z pairs)
	std::vector<glm::vec3> vertices;
	// The actual vertices in order; indices of vertices (0 for first, unlike obj files)
	std::vector<unsigned short> elements;
	// The per-face normals; less pretty and not used by default (x,y,z pairs)
	std::vector<glm::vec3> flat_normals;
	// The per-vertex normals. Only calculated once added to world. (x,y,z pairs)
	std::vector<glm::vec3> smooth_normals;
	// A set of texture coordinates, one for every element (x,y pairs). Manually set with Set_Texture_Coordinate()
	std::vector<glm::vec2> texture_coordinates;

	// Once the elements are initialized, we only need to remember the number of them
	unsigned int number_elements;

};