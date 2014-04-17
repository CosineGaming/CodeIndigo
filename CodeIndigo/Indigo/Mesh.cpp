// Defines a mesh of points for render

#include "GL/glew.h"

#include "Mesh.h"

#include "Direction.h"
#include "Indigo.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "GLFW/glfw3.h"


// Create a new, empty mesh
Mesh::Mesh(const int group_size)
{
	Hitbox = 0;
	Group_Size = group_size;
	vertices = std::vector<float>();
	elements = std::vector<unsigned short>();
	Texture_ID = -1;
	return;
}


// Create a new mesh with some vertices
Mesh::Mesh(const std::vector <glm::vec3>& add_vertices, const int group_size)
{
	vertices = std::vector<float>();
	elements = std::vector<unsigned short>();
	Add(add_vertices);
	Group_Size = group_size;
	Texture_ID = -1;
	return;
}


// Copy a mesh
Mesh::Mesh(const Mesh& mesh)
{
	vertices = mesh.vertices;
	elements = mesh.elements;
	flat_normals = mesh.flat_normals;
	smooth_normals = mesh.smooth_normals;
	Group_Size = mesh.Group_Size;
	Hitbox = mesh.Hitbox;
	Texture_ID = mesh.Texture_ID;
	return;
}


// Destroy the mesh
Mesh::~Mesh(void)
{
	return;
}


// Once added to the object, the mesh is locked into place.
void Mesh::Initialize(void)
{
	Smooth_Normals();
	glGenBuffers(1, &Vertices_ID);
	glBindBuffer(GL_ARRAY_BUFFER, Vertices_ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &Elements_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Elements_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size(), &elements[0], GL_STATIC_DRAW);
	return;
}


// The following functions are Mesh Constructors:
// A function to construct a mesh of the type with it's name.
// /*Example:*/ Mesh myCubeMesh = Mesh::Cube(1.0, 50.0, 24.2, 13.5);
Mesh Mesh::Load(const char * filename)
{
	Mesh mesh(3);
	std::ifstream file(filename, std::ios::in);
	if (!file)
	{
		std::cout << "Unable to open file " << filename << ". Replacing mesh with empty mesh." << std::endl;
	}
	else
	{
		std::string line;
		std::vector<glm::vec3> textures;
		std::vector<glm::vec3> normals;
		while (std::getline(file, line))
		{
			if (line[0] == 'v' && line[1] == ' ')
			{
				line = line.substr(2);
				const char * stream = line.c_str();
				glm::vec3 option;
				int next;
				option.x = Indigo::Fast_Float(stream, &next, 0);
				option.y = Indigo::Fast_Float(stream, &next, next);
				option.z = Indigo::Fast_Float(stream, nullptr, next);
				mesh.vertices.push_back(option.x);
				mesh.vertices.push_back(option.y);
				mesh.vertices.push_back(option.z);
			}
			if (line[0] == 'v' && line[1] == 't')
			{
				line = line.substr(3);
				const char * stream = line.c_str();
				glm::vec3 option;
				int next;
				option.x = Indigo::Fast_Float(stream, &next, 0);
				option.y = Indigo::Fast_Float(stream, nullptr, next);
				textures.push_back(option);
			}
			if (line[0] == 'v' && line[1] == 'n')
			{
				line = line.substr(3);
				const char * stream = line.c_str();
				glm::vec3 normal;
				int next;
				normal.x = Indigo::Fast_Float(stream, &next, 0);
				normal.y = Indigo::Fast_Float(stream, &next, next);
				normal.z = Indigo::Fast_Float(stream, nullptr, next);
				normals.push_back(normal);
			}
			else if (line[0] == 'f' && line[1] == ' ')
			{
				std::string values(line.substr(2));
				for (int point = 0; point < 3; ++point)
				{
					// glm::vec3
					int vertex = atoi(values.c_str()) - 1;
					mesh.elements.push_back(vertex);
					int index = mesh.Size() - 1;
					// glm::vec3 Texture
					int texturenormal = values.find('/') + 1;
					if (texturenormal != std::string::npos)
					{
						values = values.substr(texturenormal);
						if (values[0] != '/')
						{
							mesh.Set_Texture_Coordinate(index, textures[atoi(values.c_str()) - 1]);
						}
						// glm::vec3 Normal
						values = values.substr(values.find('/') + 1);
						if (point == 2 && values[0] != ' ')
						{
							int place = atoi(values.c_str()) - 1;
							if (normals.size() > place)
							{
								mesh.flat_normals.push_back(normals[place].x);
								mesh.flat_normals.push_back(normals[place].y);
								mesh.flat_normals.push_back(normals[place].z);
							}
							else
							{
								mesh.flat_normals.push_back(mesh.Find_Flat_Normal(index).x);
								mesh.flat_normals.push_back(mesh.Find_Flat_Normal(index).y);
								mesh.flat_normals.push_back(mesh.Find_Flat_Normal(index).z);
							}
						}
					}
					// Move on to next point
					values = values.substr(values.find(' ') + 1);
				}
			}
		}
		file.close();
	}
	return (mesh);
}


Mesh Mesh::Sphere(const float radius, const int recursions, const bool draw_sphere)
{
	Mesh mesh(3);
	glm::vec3 top = glm::vec3(0.0, radius, 0.0);
	Direction cursor(radius, 0.0, 0.0);
	for (int triangle = 0; triangle<8; ++triangle)
	{
		if (triangle == 4)
		{
			top = glm::vec3(0.0, -1 * radius, 0.0);
		}
		glm::vec3 left = cursor.To_Vertex();
		cursor.Add_Direction(0.0, 90.0, 0.0);
		glm::vec3 right = cursor.To_Vertex();
		mesh += Bulge_Sphere(radius, recursions, left, right, top);
	}
	return (mesh);
}


Mesh Mesh::Box(const float width, const float height, const float length)
{
	float r_width = width / 2.0;
	float r_height = height / 2.0;
	float r_length = length / 2.0;
	Mesh mesh = Mesh(4)
		// Front
		+ glm::vec3(-r_width, -r_height,  r_length)
		+ glm::vec3( r_width, -r_height,  r_length)
		+ glm::vec3( r_width,  r_height,  r_length)
		+ glm::vec3(-r_width,  r_height,  r_length)
		// Back
		+ glm::vec3(-r_width,  r_height, -r_length)
		+ glm::vec3( r_width,  r_height, -r_length)
		+ glm::vec3( r_width, -r_height, -r_length)
		+ glm::vec3(-r_width, -r_height, -r_length)
		// Left
		+ glm::vec3(-r_width, -r_height, -r_length)
		+ glm::vec3(-r_width, -r_height,  r_length)
		+ glm::vec3(-r_width,  r_height,  r_length)
		+ glm::vec3(-r_width,  r_height, -r_length)
		// Right
		+ glm::vec3( r_width,  r_height, -r_length)
		+ glm::vec3( r_width,  r_height,  r_length)
		+ glm::vec3( r_width, -r_height,  r_length)
		+ glm::vec3( r_width, -r_height, -r_length)
		// Top
		+ glm::vec3(-r_width,  r_height,  r_length)
		+ glm::vec3( r_width,  r_height,  r_length)
		+ glm::vec3( r_width,  r_height, -r_length)
		+ glm::vec3(-r_width,  r_height, -r_length)
		// Bottom
		+ glm::vec3(-r_width, -r_height, -r_length)
		+ glm::vec3( r_width, -r_height, -r_length)
		+ glm::vec3( r_width, -r_height,  r_length)
		+ glm::vec3(-r_width, -r_height,  r_length);
	return (mesh);
}


Mesh Mesh::Line(const float width, const float height, const float length)
{
	Mesh mesh;
	mesh += glm::vec3(0.0, 0.0, 0.0);
	mesh += glm::vec3(width, height, length);
	return (mesh);
}


Mesh Mesh::Cube(const float side)
{
	return (Box(side, side, side));
}


Mesh Mesh::Regular_Shape(const int sides, const float side_length)
{
	Mesh mesh(1);
	mesh += glm::vec3(0.0, 0.0, 0.0);
	Direction cursor(side_length);
	for (int point=0; point<sides; ++point)
	{
		mesh += cursor.To_Vertex();
		cursor.Add_Direction(0.0, -360.0 / sides);
	}
	return (mesh);
}


Mesh Mesh::Rectangle(const float width, const float height)
{
	Mesh mesh(4);
	mesh += glm::vec3(width / -2, height / -2, 0.0);
	mesh += glm::vec3(width / 2, height / -2, 0.0);
	mesh += glm::vec3(width / 2, height / 2, 0.0);
	mesh += glm::vec3(width / -2, height / 2, 0.0);
	return (mesh);
}


// Used in recursion for the Sphere function
Mesh Mesh::Bulge_Sphere(const float radius, const int recursions,
	const glm::vec3& left, const glm::vec3& right, const glm::vec3& top)
{
	Mesh mesh(0);
	if (recursions > 0)
	{
		glm::vec3 mid_left = left + top;
		mid_left /= 2;
		glm::vec3 mid_right = top + right;
		mid_right /= 2;
		glm::vec3 mid_top = left + right;
		mid_top /= 2;
		mesh += Bulge_Sphere(radius, recursions - 1, mid_left, mid_right, mid_top);
		mesh += Bulge_Sphere(radius, recursions - 1, mid_left, mid_right, top);
		mesh += Bulge_Sphere(radius, recursions - 1, left, mid_top, mid_left);
		mesh += Bulge_Sphere(radius, recursions - 1, mid_top, right, mid_right);
	}
	else
	{
		glm::vec3 options[3] = { left, right, top };
		for (int vertex = 0; vertex<3; ++vertex)
		{
			Direction distance = Direction(options[vertex]);
			distance.Set_Distance(radius);
			mesh += glm::vec3(distance.Get_X(), distance.Get_Y(), distance.Get_Z());
		}
	}
	return (mesh);
}


// Allows [] to get a vertex like an array
glm::vec3& Mesh::operator[](const int index) const
{
	return (Get_Vertex(index));
}


// Allows += to add a vertex to the end
Mesh& Mesh::operator+=(const glm::vec3& vertex)
{
	Add(vertex);
	return (*this);
}


// Allows += to add a mesh to the end
Mesh& Mesh::operator+=(const Mesh& mesh)
{
	for (int i = 0; i < mesh.vertices.size(); ++i)
	{
		vertices.push_back(mesh.vertices[i]);
	}
	for (int i = 0; i < mesh.flat_normals.size(); ++i)
	{
		vertices.push_back(mesh.flat_normals[i]);
	}
	for (int i = 0; i < mesh.smooth_normals.size(); ++i)
	{
		vertices.push_back(mesh.smooth_normals[i]);
	}
	if (mesh.texture_coordinates.size() != 0)
	{
		while (texture_coordinates.size()<Size() * 2)
		{
			texture_coordinates.push_back(-1);
			texture_coordinates.push_back(-1);
		}
		for (int i = 0; i < mesh.texture_coordinates.size(); ++i)
		{
			texture_coordinates.push_back(mesh.texture_coordinates[i]);
		}
	}
	for (int i = 0; i < mesh.Size(); ++i)
	{
		elements.push_back(mesh.elements[i] + Vertex_Data_Amount());
	}
	return (*this);
}


// Allows += to add a vector of vertices to the end
Mesh& Mesh::operator+=(const std::vector <glm::vec3>& add_vertices)
{
	Add(add_vertices);
	return (*this);
}


// Allows + to add a vertex to the end
Mesh Mesh::operator+(const glm::vec3& vertex) const
{
	Mesh copy = *this;
	copy.Add(vertex);
	return (copy);
}


// Allows + to add a mesh to the end
Mesh Mesh::operator+(const Mesh& mesh) const
{
	Mesh copy = *this;
	copy += mesh;
	return (copy);
}


Mesh Mesh::operator+(const std::vector <glm::vec3>& add_vertices) const
{
	// Allows + to add vertices to the end
	Mesh copy = *this;
	copy.Add(add_vertices);
	return (copy);
}


// Gets a vertex by its index
glm::vec3& Mesh::Get_Vertex(const int index) const
{
	return glm::vec3(vertices[elements[index] * 3], vertices[elements[index] * 3 + 1], vertices[elements[index] * 3 + 2]);
}


// Finds one flat normal. Assumes index is end of group.
glm::vec3 Mesh::Find_Flat_Normal(const int index) const
{
	Direction normal = Direction(Get_Vertex(index - 1)).Distance(Direction(Get_Vertex(index - 2)));
	Direction with = Direction(Get_Vertex(index)).Distance(Direction(Get_Vertex(index - 2)));
	normal = normal.Cross(with);
	normal.Normalize();
	return normal.To_Vertex();
}


// Calculate all per-vertex normals for the mesh
void Mesh::Smooth_Normals(void)
{
	smooth_normals = std::vector<float>();
	glm::vec3 * normals = new glm::vec3[Vertex_Data_Amount()];
	int * amounts = new int[Vertex_Data_Amount()];
	for (int point = 0; point < Vertex_Data_Amount(); ++point)
	{
		normals[point] = glm::vec3(0, 0, 0);
		amounts[point] = 0;
	}
	for (int point = 0; point < Size(); ++point)
	{
		normals[elements[point]] += Flat_Normal(point);
		amounts[elements[point]]++;
	}
	for (int point = 0; point < Vertex_Data_Amount(); ++point)
	{
		if (amounts[point] != 0)
		{
			normals[point].x /= amounts[point];
			normals[point].y /= amounts[point];
			normals[point].z /= amounts[point];
			smooth_normals.push_back(normals[point].x);
			smooth_normals.push_back(normals[point].y);
			smooth_normals.push_back(normals[point].z);
		}
		else
		{
			// Incorrectly ==ed, pick a fairly random but hopefully related normal
			std::cout << "Compen! Compensation! Ba-dum... Dum!" << std::endl;
			glm::vec3 add = Flat_Normal(point * Group_Size);
			smooth_normals.push_back(add.x);
			smooth_normals.push_back(add.y);
			smooth_normals.push_back(add.z);
		}
	}
	delete [] normals;
	delete [] amounts;
	return;
}



// Get the normal for a specific vertex by face
glm::vec3 Mesh::Flat_Normal(const int index) const
{
	if (index < 3 && Group_Size == 0)
	{
		if (Size() >= 3)
			return (glm::vec3(flat_normals[6], flat_normals[7], flat_normals[8]));
		else
			return (glm::vec3(0, 0, 1));
	}
	else
	{
		int start = (Group_Size != 0 ? index / Group_Size : index - 3) * 3;
		return glm::vec3(flat_normals[start], flat_normals[start + 1], flat_normals[start + 2]);
	}
}


// Get the smoother per-vertex normal for a vertex; calculate if needed
glm::vec3 Mesh::Smooth_Normal(const int index) const
{
	return glm::vec3(smooth_normals[elements[index] * 3], smooth_normals[elements[index] * 3 + 1], smooth_normals[elements[index] * 3 + 2]);
}


// Update the hitbox knowing that this point exists in the mesh
void Mesh::Update_Hitbox(glm::vec3 vertex)
{
	if (abs(vertex.x) > Hitbox)
	{
		Hitbox = abs(vertex.x);
	}
	if (abs(vertex.y) > Hitbox)
	{
		Hitbox = abs(vertex.y);
	}
	if (abs(vertex.z) > Hitbox)
	{
		Hitbox = abs(vertex.z);
	}
}


// Texture the entire mesh with one file, texture coordinates will be used only once called
void Mesh::Texture(const char * filename)
{
	std::ifstream file(filename, std::ios::binary);
	if (!file)
	{
		std::cout << "Cannot find file " << filename << " aborting texture loading." << std::endl;
		return;
	}
	char header[54];
	file.read(header, 54);
	if (header[0] != 'B' || header[1] != 'M')
	{
		std::cout << "Incorrectly configured or corrupted datafile " << filename << ". Aborting." << std::endl;
	}
	int start = *(int*) &(header[0x0A]);
	int size = *(int*) &(header[0x22]);
	int width = *(int*) &(header[0x12]);
	int height = *(int*) &(header[0x16]);
	if (size == 0)
	{
		size = width*height * 3;
	}
	if (start == 0)
	{
		start = 54;
	}
	char * data = new char[size];
	file.read(data, size);
	file.close();

	glGenTextures(1, &Texture_ID);
	glBindTexture(GL_TEXTURE_2D, Texture_ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	delete [] data;
	return;
}


// Get the coordinates of the texture, as a vertex with X and Y (and Z omitted) for a vertex
glm::vec3 Mesh::Texture_Coordinate(const int index) const
{
	if (texture_coordinates.size() < (std::size_t)(index + 1)*2)
	{
		return (glm::vec3(((index) % Group_Size) % 3 != 0, (index*3) % Group_Size < 2, 0));
	}
	else if (glm::vec3(texture_coordinates[index*2], texture_coordinates[index*2+1], -1) == glm::vec3(-1, -1, -1))
	{
		return (glm::vec3(((index) % Group_Size) % 3 != 0, (index*3) % Group_Size < 2, 0));
	}
	else
	{
		return (glm::vec3(texture_coordinates[index * 2], texture_coordinates[index * 2 + 1], 0));
	}
}


// Set the coordinates of the texture, as a vertex with X and Y (and Z omitted) for a vertex. For the special cases that the automatic isn't nice.
void Mesh::Set_Texture_Coordinate(const int index, const glm::vec3& coordinate)
{
	while (texture_coordinates.size() < (std::size_t)(index*2 + 1))
	{
		texture_coordinates.push_back(-1);
		texture_coordinates.push_back(-1);
	}
	texture_coordinates[index * 2] = coordinate.x;
	texture_coordinates[index * 2 + 1] = coordinate.y;
	return;
}



// Get the number of elements in the mesh
int Mesh::Size(void) const
{
	return (elements.size());
}


// Number of actual different vertices defined
int Mesh::Vertex_Data_Amount(void) const
{
	return (vertices.size() / 3);
}


void Mesh::Add(const glm::vec3& vertex)
{
	// Checks if this vertex has been mentioned before
	bool duplicate = false;
	for (int check = 0; check<Vertex_Data_Amount(); ++check)
	{
		if (vertex == Get_Vertex(check))
		{
			elements.push_back(check);
			duplicate = true;
		}
	}
	// Add a new vertex to the end of the mesh if not a duplicate
	if (!duplicate)
	{
		vertices.push_back(vertex.x);
		vertices.push_back(vertex.y);
		vertices.push_back(vertex.z);
		elements.push_back(Vertex_Data_Amount() - 1);
		// Update the hitbox with this new info
		Update_Hitbox(vertex);
	}
	// Calculate the light normal if this ends a face
	int point = Size() - 1;
	if (point % (Group_Size > 2 ? Group_Size : 1) == Group_Size - 1 || (Group_Size < 3 && point >= 3))
	{
		glm::vec3 normal = Find_Flat_Normal(point);
		flat_normals.push_back(normal.x);
		flat_normals.push_back(normal.y);
		flat_normals.push_back(normal.z);
	}
	return;
}


// Add new vertices to the end of the mesh
void Mesh::Add(const Mesh& mesh)
{
	for (int point = 0; point < mesh.Size(); ++point)
	{
		Add(mesh[point]);
	}
	return;
}


// Add new vertices to the end of the mesh
void Mesh::Add(const std::vector <glm::vec3>& add_vertices)
{
	for (std::size_t point = 0; point < add_vertices.size(); ++point)
	{
		Add(add_vertices[point]);
	}
	return;
}