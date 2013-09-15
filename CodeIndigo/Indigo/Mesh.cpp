// Defines a mesh of points for render

#include "Mesh.h"
#include "Vertex.h"


Mesh::Mesh (const int& group_size)
{
	// Create a new, empty mesh
	Group_Size = group_size;
	return;
}


Mesh::Mesh (const std::vector <Vertex>& add_vertices, const int& group_size)
{
	// Create a new mesh with some vertices
	Add (add_vertices);
	Group_Size = group_size;
	return;
}


Mesh::Mesh (const Mesh& mesh)
{
	// Copy a mesh
	Add(mesh.Get_Vertices());
	Group_Size = mesh.Group_Size;
	return;
}


Mesh::~Mesh (void)
{
	// Destroy the mesh
	return;
}


Mesh Mesh::Sphere (const float& radius, const int& slices, const int& stacks)
{
	Mesh mesh (0);
	Direction cursor (radius, 0.0, 0.0);
	float each_x = 360.0 / slices;
	float each_y = 360.0 / stacks;
	for (float angle_x=0.0; angle_x<=360.0; angle_x+=each_x)
	{
		for (float angle_y=0.0; angle_y<=360.0; angle_y+=each_y)
		{
			cursor.Set_Direction (radius, angle_x
				+ each_y * ((int)(angle_y / each_y + 0.5) % 2 == 0), angle_y);
			mesh += Vertex (cursor.Get_X (), cursor.Get_Y (), cursor.Get_Z ());
		}
	}
	return (mesh);
}


Mesh Mesh::Box (const float& width, const float& height, const float& length)
{
	Mesh mesh = Mesh ();
	float Half_Lengths [3] = 
	{
		width / 2.0f,
		length / 2.0f,
		height / 2.0f
	};
	int Pair [2] = {0, 1};
	mesh.Group_Size = 4;
	for (int Side=0; Side<3; Side++)
	{
		for (int Twice=0; Twice<2; Twice++)
		{
			for (int Point=0; Point<4; Point++)
			{
				Half_Lengths [Pair [Point % 2]] *= -1;
				mesh += Vertex (Half_Lengths [0], Half_Lengths [1], Half_Lengths [2]);
			}
			if (0 == Twice)
			{
				Half_Lengths [Pair [1]] *= -1;
			}
		}
		Pair [2 != Pair [1]] += 1; // 0, 1; 0, 2; 1, 2
	}
	return (mesh);
}


Mesh Mesh::Line (const float& width, const float& height, const float& length)
{
	Mesh mesh;
	mesh += Vertex (width, height, length);
	return (mesh);
}


Mesh Mesh::Cube (const float& side)
{
	return (Box (side, side, side));
}


Mesh Mesh::Regular_Shape (const int& sides, const float& side_length)
{
	return (Mesh ());
}


Mesh Mesh::Rectangle (const float& width, const float& height)
{
	return (Mesh ());
}


Vertex& Mesh::operator[] (const int& index) const
{
	// Allows [] to get a vertex like an array
	return (Get_Vertex(index));
}


Mesh& Mesh::operator+= (const Vertex& vertex)
{
	// Allows += to add a vertex to the end
	Add (vertex);
	return (*this);
}


Mesh& Mesh::operator+= (const Mesh& mesh)
{
	// Allows += to add a mesh to the end
	Add (mesh.Get_Vertices ());
	return (*this);
}


Mesh& Mesh::operator+= (const std::vector <Vertex>& add_vertices)
{
	// Allows += to add a vector of vericese to the end
	Add (add_vertices);
	return (*this);
}


Mesh Mesh::operator+ (const Vertex& vertex) const
{
	// Allows + to add a vertex to the end
	Mesh copy = *this;
	copy.Add (vertex);
	return (copy);
}


Mesh Mesh::operator+ (const Mesh& mesh) const
{
	// Allows + to add a mesh to the end
	Mesh copy = *this;
	copy.Add (mesh.Get_Vertices());
	return (copy);
}


Mesh Mesh::operator+ (const std::vector <Vertex>& add_vertices) const
{
	// Allows + to add vertices to the end
	Mesh copy = *this;
	copy.Add (add_vertices);
	return (copy);
}


void Mesh::Add_Relative (const Vertex& vertex)
{
	// Add a new vertex to the end of the mesh,
	// the last vertex made to be 0,0,0.
	// So if the last was 0,1,5, then 2,-1,3 would add 2,0,8
	Vertex add = vertex;
	Vertex last;
	if (vertices.size () != 0)
	{
		Vertex last = vertices.back ();
	}
	add += last;
	Add (add);
	return;
}


void Mesh::Add_Relative (const Mesh& mesh)
{
	Add_Relative (mesh.Get_Vertices ());
	return;
}


void Mesh::Add_Relative (const std::vector <Vertex>& add_vertices)
{
	Vertex last;
	if (add_vertices.size() != 0)
	{
		Vertex last = add_vertices.back ();
	}
	for (int Point=0; Point<add_vertices.size (); ++Point)
	{
		Vertex add = add_vertices [Point];
		add += last;
		Add (add);
	}
	return;
}


Vertex& Mesh::Get_Vertex (const int& index) const
{
	// Gets a vertex by it's index
	return (const_cast <Vertex&> (vertices [index]));
}


std::vector <Vertex> Mesh::Get_Vertices (int beginning, int end) const
{
	if (beginning < 0)
	{
		beginning += vertices.size ();
	}
	if (end < 0)
	{
		end += vertices.size ();
	}
	if (end < beginning)
	{
		end = beginning;
	}
	end += 1;
	return (std::vector <Vertex> (vertices.begin () + beginning, vertices.begin () + end));
}


int Mesh::Size (void) const
{
	return vertices.size ();
}


void Mesh::Add (const Vertex& vertex)
{
	// Add a new vertex to the end of the mesh
	vertices.push_back (vertex);
	return;
}


void Mesh::Add (const Mesh& mesh)
{
	// Add new vertices to the end of the mesh
	for (int Point=0; Point<mesh.Size (); ++Point)
	vertices.push_back (mesh [Point]);
	return;
}


void Mesh::Add (const std::vector <Vertex>& add_vertices)
{
	// Add new vertices to the end of the mesh
	for (int Point=0; Point<add_vertices.size (); ++Point)
	vertices.push_back (add_vertices [Point]);
	return;
}