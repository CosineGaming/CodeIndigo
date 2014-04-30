// Defines a mesh of points for render

#include "GL/glew.h"

#include "Mesh.h"

#include "Direction.h"
#include "Indigo.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include "GLFW/glfw3.h"


// Create a new, empty mesh
Mesh::Mesh(void)
{
	Hitbox[0] = glm::vec3(0, 0, 0);
	Hitbox[1] = glm::vec3(0, 0, 0);
	vertices = std::vector<glm::vec3>();
	elements = std::vector<unsigned short>();
	flat_normals = std::vector<glm::vec3>();
	smooth_normals = std::vector<glm::vec3>();
	texture_coordinates = std::vector<glm::vec2>();
	Color = glm::vec3(1,1,1);
	Shine = 0;
	Vertices_ID = 0;
	Elements_ID = 0;
	Texture_ID = 0;
	number_elements = 0;
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
	Hitbox[0] = mesh.Hitbox[0];
	Hitbox[1] = mesh.Hitbox[1];
	Color = mesh.Color;
	Shine = mesh.Shine;
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

	Hitbox[0] = glm::vec3(0, 0,0);
	Hitbox[1] = glm::vec3(0, 0,0);
	vertices = std::vector<glm::vec3>();
	elements = std::vector<unsigned short>();
	flat_normals = std::vector<glm::vec3>();
	smooth_normals = std::vector<glm::vec3>();
	texture_coordinates = std::vector<glm::vec2>();
	Color = color;
	Shine = shine;
	Vertices_ID = 0;
	Elements_ID = 0;
	Texture_ID = 0;
	number_elements = 0;

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
					Update_Hitbox(vertices[vertices.size() - 1]);
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

		if (texture)
		{
			Texture(texture);
		}

	}
	return;
}


// Destroy the mesh
Mesh::~Mesh(void)
{
	return;
}


// Specialized constructor for creating text
Mesh Mesh::Text(const char * text, const float size, const char * font)
{
	Mesh mesh;
	float bottom = 0;
	float top = size;
	int i = 0;
	for (char check_letter = text[0]; check_letter != '\0'; check_letter = text[++i])
	{

		int letter = check_letter - 32;
		float y = 1 - (letter / 16) / 16.0; // The most 16s you can get
		float x = (letter % 16) / 16.0; // The rest
		float left = i * size;
		float right = left + size;
		mesh.vertices.push_back(glm::vec3(left, bottom, 0)); // BL
		mesh.vertices.push_back(glm::vec3(right, bottom, 0)); // BR
		mesh.vertices.push_back(glm::vec3(right, top, 0)); // TR
		mesh.vertices.push_back(glm::vec3(left, bottom, 0)); // BL
		mesh.vertices.push_back(glm::vec3(right, top, 0)); // TR
		mesh.vertices.push_back(glm::vec3(left, top, 0)); // TL

		float tex_l = x;
		float tex_r = x + 0.0625;
		float tex_t = y;
		float tex_b = y - 0.0625;
		mesh.texture_coordinates.push_back(glm::vec2(tex_l, tex_b)); // BL
		mesh.texture_coordinates.push_back(glm::vec2(tex_r, tex_b)); // BR
		mesh.texture_coordinates.push_back(glm::vec2(tex_r, tex_t)); // TR
		mesh.texture_coordinates.push_back(glm::vec2(tex_l, tex_b)); // BL
		mesh.texture_coordinates.push_back(glm::vec2(tex_r, tex_t)); // TR
		mesh.texture_coordinates.push_back(glm::vec2(tex_l, tex_t)); // TL

		mesh.flat_normals.push_back(glm::vec3(0, 0, 1));
		mesh.flat_normals.push_back(glm::vec3(0, 0, 1));
		mesh.flat_normals.push_back(glm::vec3(0, 0, 1));
		mesh.flat_normals.push_back(glm::vec3(0, 0, 1));
		mesh.flat_normals.push_back(glm::vec3(0, 0, 1));
		mesh.flat_normals.push_back(glm::vec3(0, 0, 1));

	}
	mesh.Texture(font);
	return mesh;
}


struct Vertex_And_Texture_Coordinate
{
	glm::vec3 vertex;
	glm::vec2 texture_coordinate;
	bool operator< (const Vertex_And_Texture_Coordinate& compare) const
	{
		return memcmp(this, &compare, sizeof(Vertex_And_Texture_Coordinate)) > 0;
	}
};


// Once added to the object, the mesh is locked into place. (on the GPU)
void Mesh::Initialize(void)
{

	smooth_normals = std::vector<glm::vec3>();

	std::vector<glm::vec3> positions = std::vector<glm::vec3>();
	std::vector<glm::vec2> coordinates = std::vector<glm::vec2>();
	std::map<Vertex_And_Texture_Coordinate, unsigned short> vertex_to_index;
	for (int point = 0; point < vertices.size(); ++point)
	{
		unsigned short index = 0;
		Vertex_And_Texture_Coordinate together = { vertices[point], texture_coordinates[point] };
		std::map<Vertex_And_Texture_Coordinate, unsigned short>::iterator location = vertex_to_index.find(together);
		if (location == vertex_to_index.end())
		{
			positions.push_back(vertices[point]);
			coordinates.push_back(texture_coordinates[point]);
			unsigned short index = positions.size() - 1;
			elements.push_back(index);
			vertex_to_index[together] = index;
		}
		else
		{
			elements.push_back(location->second);
		}
	}
	smooth_normals = std::vector<glm::vec3>();
	glm::vec3 * temp_normals = new glm::vec3[positions.size()];
	int * temp_amounts = new int[positions.size()];
	for (int point = 0; point < positions.size(); ++point)
	{
		temp_normals[point] = glm::vec3(0, 0, 0);
		temp_amounts[point] = 0;
	}
	for (int point = 0; point < elements.size(); ++point)
	{
		temp_normals[elements[point]] += Flat_Normal(point);
		temp_amounts[elements[point]]++;
	}
	for (int point = 0; point < positions.size(); ++point)
	{
		if (temp_amounts[point] != 0)
		{
			temp_normals[point] /= temp_amounts[point];
			smooth_normals.push_back(temp_normals[point]);
		}
		else
		{
			// Incorrectly ==ed, pick a fairly random but hopefully related normal
			std::cout << "Compen! Compensation! Ba-dum... Dum!" << std::endl;
			smooth_normals.push_back(Flat_Normal(point * 3));
		}
	}
	delete[] temp_normals;
	delete[] temp_amounts;
	temp_normals = nullptr;
	temp_amounts = nullptr;

	texture_coordinates = coordinates;
	vertices = positions;

	glGenBuffers(1, &Vertices_ID);
	glBindBuffer(GL_ARRAY_BUFFER, Vertices_ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &Normals_ID);
	glBindBuffer(GL_ARRAY_BUFFER, Normals_ID);
	glBufferData(GL_ARRAY_BUFFER, smooth_normals.size() * sizeof(glm::vec3), &smooth_normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &UV_ID);
	glBindBuffer(GL_ARRAY_BUFFER, UV_ID);
	glBufferData(GL_ARRAY_BUFFER, texture_coordinates.size() * sizeof(glm::vec2), &texture_coordinates[0], GL_STATIC_DRAW);

	glGenBuffers(1, &Elements_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Elements_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned short), &elements[0], GL_STATIC_DRAW);

	number_elements = elements.size();

	// No need for RAM to hold it anymore, it's in VRAM! * Flush of relief as the RAM is released *
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
	if (vertex.x < Hitbox[0].x)
	{
		Hitbox[0].x = vertex.x;
	}
	if (vertex.x > Hitbox[1].x)
	{
		Hitbox[1].x = vertex.x;
	}
	if (vertex.y < Hitbox[0].y)
	{
		Hitbox[0].y = vertex.y;
	}
	if (vertex.y > Hitbox[1].y)
	{
		Hitbox[1].y = vertex.y;
	}
	if (vertex.z < Hitbox[0].z)
	{
		Hitbox[0].z = vertex.z;
	}
	if (vertex.z > Hitbox[1].z)
	{
		Hitbox[1].z = vertex.z;
	}
}


// Texture the entire mesh with one file
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

	if (Texture_ID != 0)
	{
		glDeleteTextures(1, &Texture_ID);
	}

	glGenTextures(1, &Texture_ID);
	glBindTexture(GL_TEXTURE_2D, Texture_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete [] data;

	return;
}


// Get the coordinates of the texture, as a vertex with X and Y (and Z omitted) for a vertex
glm::vec2 Mesh::Auto_Texture_Coordinate(const int index) const
{
	return (glm::vec2(index % 3 != 0, index % 3 < 2));
}



// Get the number of elements in the mesh
int Mesh::Size(void) const
{
	if (number_elements == 0)
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