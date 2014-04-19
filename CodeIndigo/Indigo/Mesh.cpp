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
Mesh::Mesh(void)
{
	Hitbox = 0;
	vertices = std::vector<glm::vec3>();
	elements = std::vector<unsigned short>();
	flat_normals = std::vector<glm::vec3>();
	smooth_normals = std::vector<glm::vec3>();
	texture_coordinates = std::vector<glm::vec2>();
	Color = glm::vec3(1,1,1);
	VAO = -1;
	Vertices_ID = -1;
	Elements_ID = -1;
	Texture_ID = -1;
	number_elements = -1;
	return;
}


// Copy a mesh
Mesh::Mesh(const Mesh& mesh)
{
	vertices = mesh.vertices;
	elements = mesh.elements;
	flat_normals = mesh.flat_normals;
	smooth_normals = mesh.smooth_normals;
	texture_coordinates = mesh.texture_coordinates;
	Hitbox = mesh.Hitbox;
	Color = mesh.Color;
	VAO = mesh.VAO;
	Vertices_ID = mesh.Vertices_ID;
	Elements_ID = mesh.Elements_ID;
	Normals_ID = mesh.Normals_ID;
	Texture_ID = mesh.Texture_ID;
	UV_ID = mesh.UV_ID;
	number_elements = mesh.number_elements;
	return;
}


// Create a new mesh by loading it from an obj file
Mesh::Mesh(const char * filename, const char * texture, const float shine, const glm::vec3& color)
{

	Hitbox = 0;
	vertices = std::vector<glm::vec3>();
	elements = std::vector<unsigned short>();
	flat_normals = std::vector<glm::vec3>();
	smooth_normals = std::vector<glm::vec3>();
	texture_coordinates = std::vector<glm::vec2>();
	Color = glm::vec3(1, 1, 1);
	VAO = -1;
	Vertices_ID = -1;
	Elements_ID = -1;
	Texture_ID = -1;
	number_elements = -1;

	std::ifstream file(filename, std::ios::in);
	if (!file)
	{
		std::cout << "Unable to open file " << filename << ". Replacing mesh with empty mesh." << std::endl;
	}
	else
	{
		std::string line;
		std::vector<glm::vec3> temp_vertices;
		std::vector<glm::vec2> temp_textures;
		std::vector<glm::vec3> temp_normals;
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
				temp_vertices.push_back(option);
			}
			if (line[0] == 'v' && line[1] == 't')
			{
				line = line.substr(3);
				const char * stream = line.c_str();
				glm::vec2 option;
				int next;
				option.x = Indigo::Fast_Float(stream, &next, 0);
				option.y = Indigo::Fast_Float(stream, nullptr, next);
				temp_textures.push_back(option);
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
				temp_normals.push_back(normal);
			}
			else if (line[0] == 'f' && line[1] == ' ')
			{
				std::string values(line.substr(2));
				for (int point = 0; point < 3; ++point)
				{
					// Vertex
					vertices.push_back(temp_vertices[atoi(values.c_str()) - 1]);
					// Vertex Texture
					int texturenormal = values.find('/') + 1;
					if (texturenormal != std::string::npos)
					{
						values = values.substr(texturenormal);
						if (values[0] != '/')
						{
							texture_coordinates.push_back(temp_textures[atoi(values.c_str()) - 1]);
						}
						// Vertex Normal
						values = values.substr(values.find('/') + 1);
						if (point == 2 && values[0] != ' ')
						{
							int place = atoi(values.c_str()) - 1;
							if (temp_normals.size() > place)
							{
								flat_normals.push_back(temp_normals[place]);
							}
							else
							{
								flat_normals.push_back(Find_Flat_Normal(Size() - 1));
							}
						}
					}
					// Move on to next point
					values = values.substr(values.find(' ') + 1);
				}
			}
		}
		file.close();
		std::cout << "Loaded " << vertices.size() << " vertices." << std::endl
			<< "Size displays as " << Size() << " with number_elements being " << number_elements << std::endl
			<< "Contains " << flat_normals.size() << " flat normals." << std::endl;
	}
	return;
}


// Destroy the mesh
Mesh::~Mesh(void)
{
	return;
}


// Once added to the object, the mesh is locked into place. (on the GPU)
void Mesh::Initialize(void)
{

	smooth_normals = std::vector<glm::vec3>();

	std::vector<unsigned short> verts = std::vector<unsigned short>();
	std::vector<glm::vec3> normals = std::vector<glm::vec3>();
	std::vector<unsigned short> amounts = std::vector<unsigned short>();
	std::vector<unsigned short> duplicate = std::vector<unsigned short>();

	for (int point = 0; point < vertices.size(); point++)
	{
		bool found = false;
		for (int check = 0; check < verts.size(); ++check)
		{
			if (vertices[verts[check]] == vertices[point])
			{
				normals[check] += Flat_Normal(point);
				amounts[check]++;
				found = true;
				break;
			}
			if (texture_coordinates[point] != texture_coordinates[point])
			{
				duplicate.push_back(check);
			}
		}
		if (!found)
		{
			verts.push_back(point);
			normals.push_back(Flat_Normal(point));
			amounts.push_back(1);
		}
	}
	for (int point = 0; point < normals.size(); ++point)
	{
		normals[point] /= amounts[point];
	}

	std::vector<glm::vec3> positions = std::vector<glm::vec3>();
	std::vector<glm::vec2> coordinates = std::vector<glm::vec2>();
	for (int point = 0; point < vertices.size(); ++point)
	{
		bool found = false;
		for (int check = 0; check < positions.size(); ++check)
		{
			if (positions[check] == vertices[point] && coordinates[check] == texture_coordinates[point])
			{
				elements.push_back(check);
				found = true;
				break;
			}
		}
		if (!found)
		{
			positions.push_back(vertices[point]);
			coordinates.push_back(texture_coordinates[point]);
			elements.push_back(positions.size() - 1);
		}
	}
	texture_coordinates = coordinates;
	vertices = positions;
	smooth_normals = normals;
	for (int i = 0; i < duplicate.size(); ++i)
	{
		smooth_normals.insert(smooth_normals.begin() + duplicate[i], smooth_normals[duplicate[i]]);
	}

	std::cout << vertices.size() << ", " << elements.size() << ", " << smooth_normals.size() << ", " << texture_coordinates.size() << std::endl;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &Vertices_ID);
	glBindBuffer(GL_ARRAY_BUFFER, Vertices_ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &Elements_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Elements_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Size() * sizeof(unsigned short), &elements[0], GL_STATIC_DRAW);

	glGenBuffers(1, &Normals_ID);
	glBindBuffer(GL_ARRAY_BUFFER, Normals_ID);
	glBufferData(GL_ARRAY_BUFFER, smooth_normals.size() * sizeof(glm::vec3), &smooth_normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &UV_ID);
	glBindBuffer(GL_ARRAY_BUFFER, UV_ID);
	glBufferData(GL_ARRAY_BUFFER, texture_coordinates.size() * sizeof(glm::vec2), &texture_coordinates[0], GL_STATIC_DRAW);

	number_elements = elements.size();

	vertices = std::vector<glm::vec3>();
	elements = std::vector<unsigned short>();
	flat_normals = std::vector<glm::vec3>();
	smooth_normals = std::vector<glm::vec3>();
	texture_coordinates = std::vector<glm::vec2>();

	return;
}


// Allows [] to get a vertex like an array
const glm::vec3& Mesh::operator[](const int index) const
{
	return (Get_Vertex(index));
}


// Allows += to add a mesh to the end
Mesh& Mesh::operator+=(const Mesh& mesh)
{
	Add(mesh);
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


// Gets a vertex by its index
const glm::vec3& Mesh::Get_Vertex(const int index) const
{
	return (vertices[index]);
}


// Gets a vertex that can be modified
glm::vec3& Mesh::Edit_Vertex(const int index)
{
	return (vertices[index]);
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


// Get the normal for a specific vertex by face
glm::vec3 Mesh::Flat_Normal(const int index) const
{
	return flat_normals[index / 3];
}


// Get the smoother per-vertex normal for a vertex; calculate if needed
glm::vec3 Mesh::Smooth_Normal(const int index) const
{
	return glm::vec3(smooth_normals[elements[index]]);
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

	if (Texture_ID != -1)
	{
		glDeleteTextures(1, &Texture_ID);
	}

	glGenTextures(1, &Texture_ID);
	glBindTexture(GL_TEXTURE_2D, Texture_ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	delete [] data;
	return;
}


// Get the coordinates of the texture, as a vertex with X and Y (and Z omitted) for a vertex
glm::vec2 Mesh::Texture_Coordinate(const int index) const
{
	if (texture_coordinates.size() < (std::size_t)index + 1)
	{
		return (glm::vec2(index % 3 != 0, index % 3 < 2));
	}
	else if (texture_coordinates[index] == glm::vec2(-1, -1))
	{
		return (glm::vec2(index % 3 != 0, index % 3 < 2));
	}
	else
	{
		return texture_coordinates[index];
	}
}



// Get the number of elements in the mesh
int Mesh::Size(void) const
{
	if (number_elements == -1)
	{
		if (elements.size())
		{
			return elements.size();
		}
		else
		{
			return vertices.size();
		}
	}
	else
	{
		return number_elements;
	}
}


// Number of actual different vertices defined
int Mesh::Vertex_Data_Amount(void) const
{
	return (vertices.size());
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
		vertices.push_back(vertex);
		texture_coordinates.push_back(glm::vec2(-1, -1));
		elements.push_back(Vertex_Data_Amount() - 1);
		// Update the hitbox with this new info
		Update_Hitbox(vertex);
	}
	// Calculate the light normal if this ends a face
	int point = Size() - 1;
	if (point % 3 == 2)
	{
		flat_normals.push_back(Find_Flat_Normal(point));
	}
	return;
}


// Add new vertices to the end of the mesh
void Mesh::Add(const Mesh& mesh)
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
		while (texture_coordinates.size()<Size())
		{
			texture_coordinates.push_back(glm::vec2(-1, -1));
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