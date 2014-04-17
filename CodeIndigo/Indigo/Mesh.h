#pragma once

#include <vector>
#include "glm/glm.hpp"


class Mesh
{

public:

	// Create a new, empty mesh
	Mesh(const int group_size=3);
	// Create a new mesh with some vertices and an optional group size
	Mesh(const std::vector <glm::vec3>& vertices, const int group_size=3);
	// Copy a mesh
	Mesh(const Mesh& mesh);
	// Destroy the mesh
	~Mesh(void);

	// Once added to the object, the mesh is locked into place
	void Initialize(void);

	// Mesh constructors:
	// A function to construct a mesh of the type with it's name.
	// /*Example*/ Mesh myCubeMesh = Mesh::Cube(1.0, 50.0, 24.2, 13.5);
	static Mesh Load(const char * filename);
	static Mesh Sphere(const float radius, const int recusions=3, const bool draw_sphere=true);
	static Mesh Box(const float width, const float height, const float length);
	static Mesh Line(const float width, const float height=0.0, const float length=0.0);
	static Mesh Cube(const float side);
	static Mesh Regular_Shape(const int sides, const float side_length);
	static Mesh Rectangle(const float width, const float height);
	// Used in recursion for the Sphere function
	static Mesh Bulge_Sphere(const float radius, const int recursions,
		const glm::vec3& left, const glm::vec3& right, const glm::vec3& top);

	// Allows [] to get a vertex like an array
	glm::vec3& operator[](const int index) const;
	// Allows += to add a vertex to the end
	Mesh& operator+=(const glm::vec3& vertex);
	// Allows += to add a mesh to the end
	Mesh& operator+=(const Mesh& mesh);
	// Allows += to add a vector of vertices to the end
	Mesh& operator+=(const std::vector <glm::vec3>& vertices);
	// Allows + to add a vertex to the end
	Mesh operator+(const glm::vec3& vertex) const;
	// Allows + to add a mesh to the end
	Mesh operator+(const Mesh& mesh) const;
	// Allows + to add a vector of vertices to the end
	Mesh operator+(const std::vector <glm::vec3>& vertices) const;

	// Gets a vertex by its index
	glm::vec3& Get_Vertex(const int index) const;

	// Calculate one flat normal. Assumes index is end of group.
	glm::vec3 Find_Flat_Normal(const int index) const;
	// Calculate all per-vertex normals for the mesh
	void Smooth_Normals(void);
	// Get the normal for a specific vertex
	glm::vec3 Flat_Normal(const int index) const;
	// Get the smoother per-vertex normal for a vertex; calculate if needed
	glm::vec3 Smooth_Normal(const int index) const;
	// Update the hitbox knowing that this point exists in the mesh
	void Update_Hitbox(glm::vec3 vertex);
	// Texture the entire mesh with one file, texture coordinates will be used only once called
	void Texture(const char * filename);
	// Get the coordinates of the texture, as a vertex with X and Y (and Z omitted) for a vertex
	glm::vec3 Texture_Coordinate(const int index) const;
	// Set the coordinates of the texture, as a vertex with X and Y (and Z omitted) for a vertex. For the special cases that the automatic isn't nice.
	void Set_Texture_Coordinate(const int index, const glm::vec3& coordinate);
	// Get the number of Vertices in the mesh

	int Size(void) const;
	// Number of actual different vertices defined
	int Vertex_Data_Amount(void) const;
	// How many sides to each polygon
	int Group_Size;

	// Hitbox used for collision, normally auto-generated. Radius of shape around center, rotation independent
	float Hitbox;
	// Add points to the mesh in function notation
	void Add(const glm::vec3& vertex);
	void Add(const Mesh& mesh);
	void Add(const std::vector <glm::vec3>& vertices);

	// The actual data is stored on the GPU. Here's the index for gathering it.
	unsigned int Vertices_ID;
	// The list of verts to use is also on the GPU. Here you yoga.
	unsigned int Elements_ID;
	// Just like the Index, but for the Texture. The texture handle number.
	unsigned int Texture_ID;

private:

	// The vertices pool to choose from. Never rendered unless in elements! (x,y,z pairs)
	std::vector<float> vertices;
	// The actual vertices in order; indices of vertices (0 for first, unlike obj files)
	std::vector<unsigned short> elements;
	// The per-face normals; less pretty and not used by default (x,y,z pairs)
	std::vector<float> flat_normals;
	// The per-vertex normals. Only calculated once added to world. (x,y,z pairs)
	std::vector<float> smooth_normals;
	// A set of texture coordinates, one for every element (x,y pairs). Manually set with Set_Texture_Coordinate()
	std::vector<float> texture_coordinates;

};