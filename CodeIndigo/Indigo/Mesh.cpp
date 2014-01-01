// Defines a mesh of points for render

#include "Mesh.h"
#include "Direction.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>


Mesh::Mesh(const int group_size)
{
	// Create a new, empty mesh
	Hitbox[0] = Vertex(0, 0, 0);
	Hitbox[1] = Vertex(0, 0, 0);
	Group_Size = group_size;
	return;
}


Mesh::Mesh(const std::vector <Vertex>& add_vertices, const int group_size)
{
	// Create a new mesh with some vertices
	Add(add_vertices);
	Group_Size = group_size;
	return;
}


Mesh::Mesh(const Mesh& mesh)
{
	// Copy a mesh
	Add(mesh.Get_Vertices());
	Group_Size = mesh.Group_Size;
	return;
}


Mesh::~Mesh(void)
{
	// Destroy the mesh
	return;
}


Mesh Mesh::Load(const char * filename)
{
	Mesh mesh(3);
	std::ifstream file(filename, std::ios::in);
	if (!file)
	{
		std::cout << "Unable to open file " << filename << ". Replacing mesh with empty mesh.\n";
	}
	else
	{
		std::vector<Vertex> options;
		std::string line;
		while (std::getline(file, line))
		{
			if (line[0] == 'v' && line[1] == ' ')
			{
				std::istringstream stream(line.substr(2));
				Vertex option;
				stream >> option.X;
				stream >> option.Y;
				stream >> option.Z;
				options.push_back(option);
			}
			else if (line[0] == 'f' && line[1] == ' ')
			{
				std::string values(line.substr(2));
				int face[3];
				face[0] = atoi(values.c_str());
				values = values.substr(values.find(' ') + 1);
				face[1] = atoi(values.c_str());
				values = values.substr(values.find(' ') + 1);
				face[2] = atoi(values.c_str());
				face[0] -= 1;
				face[1] -= 1;
				face[2] -= 1;
				mesh += options[face[0]];
				mesh += options[face[1]];
				mesh += options[face[2]];
			}
		}
		file.close();
	}
	return mesh;
}


Mesh Mesh::Sphere(const float radius, const int recursions, const bool draw_sphere)
{
	Mesh mesh(3);
	Vertex top = Vertex(0.0, radius, 0.0);
	Direction cursor(radius, 0.0, 0.0);
	for (int triangle = 0; triangle<8; ++triangle)
	{
		if (triangle == 4)
		{
			top = Vertex(0.0, -1 * radius, 0.0);
		}
		Vertex left = cursor.To_Vertex();
		cursor.Add_Direction(0.0, 90.0, 0.0);
		Vertex right = cursor.To_Vertex();
		mesh += Bulge_Sphere(radius, recursions, left, right, top);
	}
	return mesh;
}


Mesh Mesh::Box(const float width, const float height, const float length)
{
	Mesh mesh = Mesh();
	float Half_Lengths[3] =
	{
		width / 2.0f,
		height / 2.0f,
		length / 2.0f
	};
	int Pair[2] = { 0, 1 };
	mesh.Group_Size = 4;
	for (int Side = 0; Side<3; Side++)
	{
		for (int Twice = 0; Twice<2; Twice++)
		{
			for (int Point = 0; Point<4; Point++)
			{
				Half_Lengths[Pair[Point % 2]] *= -1;
				mesh += Vertex(Half_Lengths[0],
					Half_Lengths[1], Half_Lengths[2]);
			}
			if (0 == Twice)
			{
				Half_Lengths[0] *= -1;
				Half_Lengths[1] *= -1;
				Half_Lengths[2] *= -1;
				Half_Lengths[Pair[0]] *= -1;
			}
		}
		Pair[2 != Pair[1]] += 1; // 0, 1; 0, 2; 1, 2
	}
	return mesh;
}


Mesh Mesh::Line(const float width, const float height, const float length)
{
	Mesh mesh;
	mesh += Vertex(0.0, 0.0, 0.0);
	mesh += Vertex(width, height, length);
	return mesh;
}


Mesh Mesh::Cube(const float side)
{
	return Box(side, side, side);
}


Mesh Mesh::Regular_Shape(const int sides, const float side_length)
{
	Mesh mesh(1);
	mesh += Vertex(0.0, 0.0, 0.0);
	Direction cursor(side_length);
	for (; cursor.Get_X_Angle() <= 360.0; cursor.Add_Direction(0.0, 360.0 / sides))
	{
		mesh += cursor.To_Vertex();
	}
	return mesh;
}


Mesh Mesh::Rectangle(const float width, const float height)
{
	Mesh mesh(4);
	mesh += Vertex(width / -2, height / -2, 0.0);
	mesh += Vertex(width / 2, height / -2, 0.0);
	mesh += Vertex(width / 2, height / 2, 0.0);
	mesh += Vertex(width / -2, height / 2, 0.0);
	return mesh;
}


// Used in recursion for the Sphere function
Mesh Mesh::Bulge_Sphere(const float radius, const int recursions,
	const Vertex& left, const Vertex& right, const Vertex& top)
{
	Mesh mesh(0);
	if (recursions > 0)
	{
		Vertex mid_left = left.Midpoint(top);
		Vertex mid_right = top.Midpoint(right);
		Vertex mid_top = left.Midpoint(right);
		mesh += Bulge_Sphere(radius, recursions - 1, mid_left, mid_right, mid_top);
		mesh += Bulge_Sphere(radius, recursions - 1, mid_left, mid_right, top);
		mesh += Bulge_Sphere(radius, recursions - 1, left, mid_top, mid_left);
		mesh += Bulge_Sphere(radius, recursions - 1, mid_top, right, mid_right);
	}
	else
	{
		Vertex vertices[3] = { left, right, top };
		for (int vertex = 0; vertex<3; ++vertex)
		{
			Direction distance = vertices[vertex].To_Direction();
			distance.Set_Direction(radius, distance.Get_X_Angle(),
				distance.Get_Y_Angle());
			mesh += Vertex(distance.Get_X(), distance.Get_Y(), distance.Get_Z());
		}
	}
	return mesh;
}


Vertex& Mesh::operator[](const int index) const
{
	// Allows [] to get a vertex like an array
	return Get_Vertex(index);
}


Mesh& Mesh::operator+=(const Vertex& vertex)
{
	// Allows += to add a vertex to the end
	Add(vertex);
	return *this;
}


Mesh& Mesh::operator+=(const Mesh& mesh)
{
	// Allows += to add a mesh to the end
	Add(mesh.Get_Vertices());
	return *this;
}


Mesh& Mesh::operator+=(const std::vector <Vertex>& add_vertices)
{
	// Allows += to add a vector of vericese to the end
	Add(add_vertices);
	return *this;
}


Mesh Mesh::operator+(const Vertex& vertex) const
{
	// Allows + to add a vertex to the end
	Mesh copy = *this;
	copy.Add(vertex);
	return copy;
}


Mesh Mesh::operator+(const Mesh& mesh) const
{
	// Allows + to add a mesh to the end
	Mesh copy = *this;
	copy.Add(mesh.Get_Vertices());
	return copy;
}


Mesh Mesh::operator+(const std::vector <Vertex>& add_vertices) const
{
	// Allows + to add vertices to the end
	Mesh copy = *this;
	copy.Add(add_vertices);
	return copy;
}


void Mesh::Add_Relative(const Vertex& vertex)
{
	// Add a new vertex to the end of the mesh,
	// the last vertex made to be 0,0,0.
	// So if the last was 0,1,5, then 2,-1,3 would add 2,0,8
	Vertex add = vertex;
	Vertex last;
	if (vertices.size() != 0)
	{
		Vertex last = vertices.back();
	}
	add += last;
	Add(add);
	return;
}


void Mesh::Add_Relative(const Mesh& mesh)
{
	Add_Relative(mesh.Get_Vertices());
	return;
}


void Mesh::Add_Relative(const std::vector <Vertex>& add_vertices)
{
	Vertex last;
	if (add_vertices.size() != 0)
	{
		Vertex last = add_vertices.back();
	}
	for (int Point = 0; Point<add_vertices.size(); ++Point)
	{
		Vertex add = add_vertices[Point];
		add += last;
		Add(add);
	}
	return;
}


Vertex& Mesh::Get_Vertex(const int index) const
{
	// Gets a vertex by it's index
<<<<<<< HEAD
	return const_cast <Vertex&>(vertices[elements[index]]);
=======
	return const_cast <Vertex&>(vertices[index]);
>>>>>>> 3c710a7217e3c1084d4d4bd763e41d36d11f035d
}


std::vector<Vertex> Mesh::Get_Vertices(int beginning, int end) const
{
	if (beginning < 0)
	{
		beginning += vertices.size();
	}
	if (end < 0)
	{
		end += vertices.size();
	}
	if (end < beginning)
	{
		end = beginning;
	}
	end += 1;
	return std::vector<Vertex>(vertices.begin() + beginning, vertices.begin() + end);
}


// Calculate all per-vertex normals for the mesh
void Mesh::Smooth_Normals(void)
{
	std::vector<Vertex> normals;
	std::vector<int> amounts;
	for (int point = 0; point < Vertex_Data_Amount(); ++point)
	{
		normals.push_back(Vertex());
		amounts.push_back(0);
	}
	for (int point = 0; point < Size(); ++point)
	{
		normals[elements[point]] += Flat_Normal(point);
		amounts[elements[point]]++;
	}
	for (int point = 0; point < normals.size(); ++point)
	{
		normals[point].X /= amounts[point];
		normals[point].Y /= amounts[point];
		normals[point].Z /= amounts[point];
		smooth_normals.push_back(normals[point]);
	}
}


// Get the normal for a specific vertex by face
Vertex Mesh::Flat_Normal(const int index) const
{
	return flat_normals[Group_Size != 0 ? index / Group_Size : index - 3];
}


// Get the smoother per-vertex normal for a vertex; calculate if needed
Vertex Mesh::Smooth_Normal(const int index)
{
	return smooth_normals[elements[index]];
}


// Get the number of elements in the mesh
int Mesh::Size(void) const
{
	return elements.size();
}


// Number of actual different vertices defined
int Mesh::Vertex_Data_Amount(void) const
{
	return vertices.size();
}


void Mesh::Add(const Vertex& vertex)
{
	// Checks if this vertex has been mentioned before
	bool duplicate = false;
	for (int Check = 0; Check<Size(); ++Check)
	{
		if (vertex == vertices[Check])
		{
			elements.push_back(Check);
			duplicate = true;
		}
	}
	// Add a new vertex to the end of the mesh if not a duplicate
	if (!duplicate)
	{
		vertices.push_back(vertex);
		elements.push_back(Size() - 1);
	}
	// Calculate the light normal if this ends a face
	int point = Size() - 1;
	if (point % (Group_Size > 2 ? Group_Size : 1) == Group_Size - 1 || (Group_Size < 3 && point >= 3))
	{
		Direction normal = Get_Vertex(point - 1).To_Direction().Distance(Get_Vertex(point - 2).To_Direction());
		Direction with = Get_Vertex(point).To_Direction().Distance(Get_Vertex(point - 2).To_Direction());
		normal = normal.Cross(with);
		normal.Normalize();
		with = Direction::Coordinates(Get_Vertex(point - 2).X * -1, Get_Vertex(point - 2).Y * -1, Get_Vertex(point - 2).Z * -1);
		if (normal.Dot(with) > 0)
		{
			Vertex value = normal.To_Vertex();
			flat_normals.push_back(Vertex(value.X * -1, value.Y * -1, value.Z * -1));
		}
		else
		{
			flat_normals.push_back(normal.To_Vertex());
		}
	}
	// Update the hitbox with the new vertex
	if (vertex.X < Hitbox[0].X)
	{
		Hitbox[0].X = vertex.X;
	}
	if (vertex.Y < Hitbox[0].Y)
	{
		Hitbox[0].Y = vertex.Y;
	}
	if (vertex.Z < Hitbox[0].Z)
	{
		Hitbox[0].Z = vertex.Z;
	}
	if (vertex.X > Hitbox[1].X)
	{
		Hitbox[1].X = vertex.X;
	}
	if (vertex.Y > Hitbox[1].Y)
	{
		Hitbox[1].Y = vertex.Y;
	}
	if (vertex.Z > Hitbox[1].Z)
	{
		Hitbox[1].Z = vertex.Z;
	}
	return;
}


void Mesh::Add(const Mesh& mesh)
{
	// Add new vertices to the end of the mesh
	for (int Point = 0; Point < mesh.Size(); ++Point)
	{
		Add(mesh[Point]);
	}
	return;
}


void Mesh::Add(const std::vector <Vertex>& add_vertices)
{
	// Add new vertices to the end of the mesh
	for (int Point = 0; Point < add_vertices.size(); ++Point)
	{
		Add(add_vertices[Point]);
	}
	return;
}