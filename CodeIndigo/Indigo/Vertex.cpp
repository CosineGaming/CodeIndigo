// Defines a three-dimensional position

#include "Vertex.h"


Vertex::Vertex (void)
{
	X = 0.0;
	Y = 0.0;
	Z = 0.0;
	return;
}


Vertex::Vertex (const Vertex& vertex)
{
	X = vertex.X;
	Y = vertex.Y;
	Z = vertex.Z;
	return;
}


Vertex::Vertex (const float& x, const float& y, const float& z)
{
	X = x;
	Y = y;
	Z = z;
	return;
}


Vertex::~Vertex (void)
{
	return;
}


Vertex& Vertex::operator+= (const Vertex& vertex)
{
	X += vertex.X;
	Y += vertex.Y;
	Z += vertex.Z;
	return *this;
}


Mesh Vertex::operator+ (const Vertex& vertex) const
{
	Mesh mesh;
	mesh += *this;
	mesh += vertex;
	return (mesh);
}            