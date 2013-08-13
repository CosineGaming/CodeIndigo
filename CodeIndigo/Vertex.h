#pragma once

#include "Mesh.h"


class Vertex
{
public:
	// Create a new, blank vertex
	Vertex (void);
	// Copies a vertex
	Vertex (const Vertex& vertex);
	// Create a vertex with these values
	Vertex (const float& x, const float& y, const float& z);
	// Destroy a vertex
	~Vertex (void);
	// Enables += to add the X, Y, and Z values to a vertex
	Vertex& operator+= (const Vertex& vertex);
	// Enables + to put values together into a mesh
	Mesh operator+ (const Vertex& vertex) const;
	// Position of the vertex
	float X;
	float Y;
	float Z;
protected:
private:
};