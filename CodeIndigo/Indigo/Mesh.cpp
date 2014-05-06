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
#include <set>
#include "GLFW/glfw3.h"


// Create a new, empty mesh
Mesh::Mesh(void)
{
	Hitbox[0] = glm::vec3(0, 0, 0);
	Hitbox[1] = glm::vec3(0, 0, 0);
	Color = glm::vec4(1,1,1,1);
	Shine = 0;
	Vertices_ID = 0;
	Elements_ID = 0;
	Texture_ID = 0;
	Size = 0;
	return;
}


// Copy a mesh
Mesh::Mesh(const Mesh& mesh)
{
	Hitbox[0] = mesh.Hitbox[0];
	Hitbox[1] = mesh.Hitbox[1];
	Color = mesh.Color;
	Shine = mesh.Shine;
	Vertices_ID = mesh.Vertices_ID;
	Elements_ID = mesh.Elements_ID;
	Normals_ID = mesh.Normals_ID;
	Texture_ID = mesh.Texture_ID;
	UV_ID = mesh.UV_ID;
	Size = mesh.Size;
	return;
}


// Create a new mesh by loading it from an obj file
Mesh::Mesh(const char * filename, const char * texture, const float shine, const glm::vec4& color)
{

	Hitbox[0] = glm::vec3(0, 0,0);
	Hitbox[1] = glm::vec3(0, 0,0);
	Color = color;
	Shine = shine;
	Vertices_ID = 0;
	Elements_ID = 0;
	Texture_ID = 0;
	Size = 0;

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
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> textures;
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
							textures.push_back(temp_textures[atoi(values.c_str()) - 1]);
						}
						else
						{
							textures.push_back(glm::vec2(0, 0));
						}
						// Vertex Normal
						values = values.substr(values.find('/') + 1);
						if (point == 2 && values[0] != ' ')
						{
							int place = atoi(values.c_str()) - 1;
							if (temp_normals.size() > place)
							{
								normals.push_back(temp_normals[place]);
							}
							else
							{
								normals.push_back(glm::cross(vertices[point - 1] - vertices[point - 2], vertices[point] - vertices[point - 2]));
							}
						}
					}
					else
					{
						textures.push_back(glm::vec2(0, 0));
						normals.push_back(glm::cross(vertices[point - 1] - vertices[point - 2], vertices[point] - vertices[point - 2]));
					}
					// Move on to next point
					values = values.substr(values.find(' ') + 1);
				}
			}
		}
		file.close();

		Texture(texture);

		Initialize(vertices, textures, normals);

	}
	return;
}


// Destroy the mesh
Mesh::~Mesh(void)
{
	return;
}


// Specialized constructor for creating text
Mesh Mesh::Text(const char * text, const float size, const char * font, const glm::vec4& color)
{
	Mesh mesh;
	mesh.Color = color;
	float bottom = 0;
	float top = size;
	float left = 0;
	float right = size;
	int i = 0;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	for (char check_letter = text[0]; check_letter != '\0'; check_letter = text[++i])
	{

		int letter = check_letter - 32;
		float y = 1 - (letter / 16) / 16.0; // The most 16s you can get
		float x = (letter % 16) / 16.0; // The rest
		vertices.push_back(glm::vec3(left, bottom, 0)); // BL
		vertices.push_back(glm::vec3(right, bottom, 0)); // BR
		vertices.push_back(glm::vec3(right, top, 0)); // TR
		vertices.push_back(glm::vec3(left, bottom, 0)); // BL
		vertices.push_back(glm::vec3(right, top, 0)); // TR
		vertices.push_back(glm::vec3(left, top, 0)); // TL

		float tex_l = x;
		float tex_r = x + 0.0625;
		float tex_t = y;
		float tex_b = y - 0.0625;
		uvs.push_back(glm::vec2(tex_l, tex_b)); // BL
		uvs.push_back(glm::vec2(tex_r, tex_b)); // BR
		uvs.push_back(glm::vec2(tex_r, tex_t)); // TR
		uvs.push_back(glm::vec2(tex_l, tex_b)); // BL
		uvs.push_back(glm::vec2(tex_r, tex_t)); // TR
		uvs.push_back(glm::vec2(tex_l, tex_t)); // TL

		normals.push_back(glm::vec3(0, 0, 1));
		normals.push_back(glm::vec3(0, 0, 1));

		left += size;
		right += size;
		if (check_letter == '\n')
		{
			bottom -= size;
			top -= size;
			left = 0;
			right = size;
		}

	}
	mesh.Initialize(vertices, uvs, normals);
	mesh.Texture(font);
	//mesh.Initialize();
	return mesh;
}


// Used by Initialize's maps
struct Vertex_Texture_Normal
{
	glm::vec3 vertex;
	glm::vec2 texture;
	glm::vec3 normal;
	bool operator< (const Vertex_Texture_Normal& compare) const
	{
		return memcmp(this, &compare, sizeof(Vertex_Texture_Normal)) > 0;
	}
};


// Used by Initialize's maps
struct Vertex_Normal
{
	glm::vec3 vertex;
	glm::vec3 normal;
	bool operator< (const Vertex_Normal& compare) const
	{
		return memcmp(this, &compare, sizeof(Vertex_Normal)) > 0;
	}
};


struct Vertex_Compare
{
	glm::vec3 vertex;
	bool operator< (const Vertex_Compare& compare) const
	{
		return memcmp(this, &compare, sizeof(Vertex_Compare)) > 0;
	}
};


// Once added to the object, the mesh is locked into place. (on the GPU)
void Mesh::Initialize(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& uvs, const std::vector<glm::vec3>& normals)
{

	// Smooth the normals
	std::vector<unsigned short> normal_indices;
	std::vector<glm::vec3> vertex_normals;
	std::map<Vertex_Compare, unsigned short> vertex_to_normal;
	std::cout << normals.size() << ", " << vertices.size() << std::endl;
	for (int vertex = 0; vertex < vertices.size(); ++vertex)
	{
		std::map<Vertex_Compare, unsigned short>::iterator location = vertex_to_normal.find({ vertices[vertex] });
		if (location == vertex_to_normal.end())
		{
			vertex_normals.push_back(normals[vertex / 3]);
			vertex_to_normal[{vertices[vertex]}] = vertex_normals.size() - 1;
			normal_indices.push_back(vertex_normals.size() - 1);
		}
		else
		{
			vertex_normals[location->second] += normals[vertex / 3];
			normal_indices.push_back(location->second);
		}
	}
	for (int normal = 0; normal < vertex_normals.size(); ++normal)
	{
		vertex_normals[normal] = glm::normalize(vertex_normals[normal]);
	}
	std::cout << vertex_normals.size() << std::endl;

	// Index for the VBO!
	std::vector<glm::vec3> final_vertices = std::vector<glm::vec3>();
	std::vector<glm::vec2> final_uvs = std::vector<glm::vec2>();
	std::vector<glm::vec3> final_normals = std::vector<glm::vec3>();
	std::vector<unsigned short> elements = std::vector<unsigned short>();
	std::map<Vertex_Texture_Normal, unsigned short> vertex_to_index;
	for (int point = 0; point < vertices.size(); ++point)
	{
		unsigned short index = 0;
		Vertex_Texture_Normal together = { vertices[point], uvs[point], vertex_normals[point] };
		std::map<Vertex_Texture_Normal, unsigned short>::iterator location = vertex_to_index.find(together);
		if (location == vertex_to_index.end())
		{
			final_vertices.push_back(vertices[point]);
			final_uvs.push_back(uvs[point]);
			final_normals.push_back(vertex_normals[normal_indices[point]]);
			unsigned short index = final_vertices.size() - 1;
			elements.push_back(index);
			vertex_to_index[together] = index;
		}
		else
		{
			elements.push_back(location->second);
		}
	}
	std::cout << final_vertices.size() << ", " << final_uvs.size() << ", " << elements.size() << std::endl;

	// Let's do it! It's ready! Let's send it to the GPU!
	glGenBuffers(1, &Vertices_ID);
	glBindBuffer(GL_ARRAY_BUFFER, Vertices_ID);
	glBufferData(GL_ARRAY_BUFFER, final_vertices.size() * sizeof(glm::vec3), &final_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &Normals_ID);
	glBindBuffer(GL_ARRAY_BUFFER, Normals_ID);
	glBufferData(GL_ARRAY_BUFFER, final_normals.size() * sizeof(glm::vec3), &final_normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &UV_ID);
	glBindBuffer(GL_ARRAY_BUFFER, UV_ID);
	glBufferData(GL_ARRAY_BUFFER, final_uvs.size() * sizeof(glm::vec2), &final_uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &Elements_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Elements_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned short), &elements[0], GL_STATIC_DRAW);

	// Oh, yeah, we actually need to render this. How big was this model, again?
	Size = elements.size();
	std::cout << (final_vertices.size() * 4 + elements.size() * 1) * 100 / (elements.size() * 4) << "% of original size.\n";

	return;
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
	char * data;
	int width;
	int height;
	if (!filename)
	{
		data = new char[3];
		data[0] = 255;
		data[1] = 255;
		data[2] = 255;
		width = 1;
		height = 1;
	}
	else
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
		width = *(int*) &(header[0x12]);
		height = *(int*) &(header[0x16]);
		if (size == 0)
		{
			size = width*height * 3;
		}
		if (start == 0)
		{
			start = 54;
		}
		data = new char[size];
		file.read(data, size);
		file.close();
	}

	if (Texture_ID != 0)
	{
		glDeleteTextures(1, &Texture_ID);
	}

	glGenTextures(1, &Texture_ID);
	glBindTexture(GL_TEXTURE_2D, Texture_ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	delete [] data;

	return;
}