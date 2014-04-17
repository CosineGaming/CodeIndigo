// Defines a three-dimensional position

/*#include "Direction.h"
#include "glm::vec3.h"
#include "Mesh.h"


// Create a new, blank vertex
glm::vec3::glm::vec3(void)
{
	X = 0.0;
	Y = 0.0;
	Z = 0.0;
	return;
}


// Copies a vertex
glm::vec3::glm::vec3(const glm::vec3& vertex)
{
	X = vertex.X;
	Y = vertex.Y;
	Z = vertex.Z;
	return;
}


// Create a vertex with these values
glm::vec3::glm::vec3(const float x, const float y, const float z)
{
	X = x;
	Y = y;
	Z = z;
	return;
}


// Destroy a vertex
glm::vec3::~glm::vec3(void)
{
	return;
}


// Enables += to add the X, Y, and Z values to a vertex
glm::vec3& glm::vec3::operator+=(const glm::vec3& vertex)
{
	X += vertex.X;
	Y += vertex.Y;
	Z += vertex.Z;
	return (*this);
}


// Enables + to put values together into a mesh
Mesh glm::vec3::operator+(const glm::vec3& vertex) const
{
	Mesh mesh;
	mesh += *this;
	mesh += vertex;
	return (mesh);
}


// Checks whether this glm::vec3 is exactly the same as another
bool glm::vec3::operator==(const glm::vec3& check) const
{
	return (check.X == this->X && check.Y == this->Y && check.Z == this->Z);
}


// Finds the midpoint between 2 vertices, useful for triangular recursion
glm::vec3 glm::vec3::Midpoint(const glm::vec3& vertex) const
{
	glm::vec3 copy = *this;
	copy.X = (X + vertex.X) / 2;
	copy.Y = (Y + vertex.Y) / 2;
	copy.Z = (Z + vertex.Z) / 2;
	return (copy);
}


// Returns a direction with the coordinates of the vertex
Direction glm::vec3::To_Direction(void) const
{
	return (Direction::Coordinates(X, Y, Z));
}
*/