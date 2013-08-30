#pragma once

#include <vector>
#include "Direction.h"

class Vertex;


class Mesh
{
public:
	// Create a new, empty mesh
	Mesh (const int& group_size=3);
	// Create a new mesh with some vertices and an optional group size
	Mesh (const std::vector <Vertex>& vertices, const int& group_size=3);
	// Copy a mesh
	Mesh (const Mesh& mesh);
	// Destroy the mesh
	~Mesh (void);
	// Mesh constructors:
	// A function to construct a mesh of the type with it's name.
	// /*Example*/ Mesh myCubeMesh = Mesh::Cube (1.0, 50.0, 24.2, 13.5);
	static Mesh Sphere (const float& radius, const int& slices=25, const int& stacks=25);
	static Mesh Box (const float& width, const float& height, const float& length);
	static Mesh Line (const float& width, const float& height=0.0, const float& length=0.0);
	static Mesh Cube (const float& side);
	static Mesh Regular_Shape (const int& sides, const float& side_length);
	static Mesh Rectangle (const float& width, const float& height);
	// Allows [] to get a vertex like an array
	Vertex& operator[] (const int& index) const;
	// Allows += to add a vertex to the end
	Mesh& operator+= (const Vertex& vertex);
	// Allows += to add a mesh to the end
	Mesh& operator+= (const Mesh& mesh);
	// Allows += to add a vector of vertices to the end
	Mesh& operator+= (const std::vector <Vertex>& vertices);
	// Allows + to add a vertex to the end
	Mesh operator+ (const Vertex& vertex) const;
	// Allows + to add a mesh to the end
	Mesh operator+ (const Mesh& mesh) const;
	// Allows + to add a vector of vertices to the end
	Mesh operator+ (const std::vector <Vertex>& vertices) const;
	// Add a new vertex to the end of the mesh,
	// the last vertex made to be 0,0,0.
	// So if the last was 0,1,5, then 2,-1,3 would add 2,0,8
	void Add_Relative (const Vertex& vertex);
	// Add a new mesh to the end relative to the last point
	void Add_Relative (const Mesh& mesh);
	// Add a new set of vertices to the end relative to the last point
	void Add_Relative (const std::vector <Vertex>& vertices);
	// Gets a vertex by it's index
	Vertex& Get_Vertex (const int& index) const;
	// Get all the vertices, or a subset of them
	std::vector <Vertex> Get_Vertices (int beginning=0, int end=-1) const;
	// Get the number of Vertices in the mesh
	int Size (void) const;
	// How many sides to each polygon
	int Group_Size;
	// Add points to the mesh in function notation
	void Add (const Vertex& vertex);
	void Add (const Mesh& mesh);
	void Add (const std::vector <Vertex>& vertices);
protected:
	// The actual vertices
	std::vector <Vertex> vertices;
private:
};