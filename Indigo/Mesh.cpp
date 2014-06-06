// Defines a mesh of points for render

#include "GL/glew.h"

#include "Mesh.h"

#include "Direction.h"
#include "Indigo.h"
#include "External/stb_image.c"

#include <fstream>
#include <iostream>
#include "GLFW/glfw3.h"


// Create a new, empty mesh
Mesh::Mesh(void) :
	Vertices_ID(0),
	Elements_ID(0),
	Normals_ID(0),
	Bump_X_Normals_ID(0),
	Bump_Y_Normals_ID(0),
	UV_ID(0),
	Texture_ID(0),
	Size(0),
	Obj_File_Hash(0),
	Texture_File_Hash(0),
	Vertex_References(new unsigned short),
	Texture_References(new unsigned short)
{
	Hitbox[0] = glm::vec3(0, 0, 0);
	Hitbox[1] = glm::vec3(0, 0, 0);
	*Vertex_References = 1;
	*Texture_References = 1;
	return;
}


// Copy a mesh
Mesh::Mesh(const Mesh& mesh) : 
	Vertices_ID(mesh.Vertices_ID),
	Elements_ID(mesh.Elements_ID),
	Normals_ID(mesh.Normals_ID),
	Bump_X_Normals_ID(mesh.Bump_X_Normals_ID),
	Bump_Y_Normals_ID(mesh.Bump_Y_Normals_ID),
	UV_ID(mesh.UV_ID),
	Texture_ID(mesh.Texture_ID),
	Size(mesh.Size),
	Obj_File_Hash(mesh.Obj_File_Hash),
	Texture_File_Hash(mesh.Texture_File_Hash),
	Vertex_References(mesh.Vertex_References),
	Texture_References(mesh.Texture_References)
{
	Hitbox[0] = mesh.Hitbox[0];
	Hitbox[1] = mesh.Hitbox[1];
	*Vertex_References += 1; // Copied once
	*Texture_References += 1;
	return;
}


// Assign a mesh. Resource management -- Needs to be wibbled and yall
Mesh& Mesh::operator=(const Mesh& mesh)
{
	if (this != &mesh)
	{
		this->~Mesh(); // This one is dying, as much as it's sad to admit it.
		Vertex_References = mesh.Vertex_References; // But there's a new one!
		Texture_References = mesh.Texture_References;
		*Vertex_References += 1; // Copied once
		*Texture_References += 1;
		Hitbox[0] = mesh.Hitbox[0];
		Hitbox[1] = mesh.Hitbox[1];
		Vertices_ID = mesh.Vertices_ID;
		Elements_ID = mesh.Elements_ID;
		Normals_ID = mesh.Normals_ID;
		Bump_X_Normals_ID = mesh.Bump_X_Normals_ID;
		Bump_Y_Normals_ID = mesh.Bump_Y_Normals_ID;
		UV_ID = mesh.UV_ID;
		Texture_ID = mesh.Texture_ID;
		Size = mesh.Size;
	}
	return *this;
}


// Destroy the mesh. Free the GPU resources if it's the last one.
Mesh::~Mesh(void)
{
	*Vertex_References -= 1; // Destroyed once
	if (*Vertex_References == 0)
	{
		// We're the last, lonely reference.
		if (Vertices_ID)
		{
			glDeleteBuffers(1, &Vertices_ID);
		}
		if (Elements_ID)
		{
			glDeleteBuffers(1, &Elements_ID);
		}
		if (Normals_ID)
		{
			glDeleteBuffers(1, &Normals_ID);
		}
		if (Bump_X_Normals_ID)
		{
			glDeleteBuffers(1, &Bump_X_Normals_ID);
		}
		if (Bump_Y_Normals_ID)
		{
			glDeleteBuffers(1, &Bump_Y_Normals_ID);
		}
		if (UV_ID)
		{
			glDeleteBuffers(1, &UV_ID);
		}
		if (Obj_File_Hash)
		{
			Load_Once[Obj_File_Hash] = std::vector<unsigned int>();
		}
		delete Vertex_References;
	}
	if (*Texture_References == 0)
	{
		if (Texture_ID)
		{
			glDeleteTextures(1, &Texture_ID);
		}
		if (Texture_File_Hash)
		{
			Load_Once[Texture_File_Hash] = std::vector<unsigned int>();
		}
		delete Texture_References;
	}
	return;
}


// Create a new mesh by loading it from an obj file
Mesh::Mesh(const char * filename, const char * texture) :
	Mesh()
{

	for (int i = 0; filename[i] != '\0'; ++i)
	{
		Obj_File_Hash += int(filename[i]);
		Obj_File_Hash %= 4294967291; // Largest prime < 2^32
	}
	std::map<unsigned int, std::vector<unsigned int>>::iterator location = Load_Once.find(Obj_File_Hash);
	if (location != Load_Once.end() && location->second.size())
	{
		std::vector<unsigned int>& items = location->second;
		Vertices_ID = items[0];
		Elements_ID = items[1];
		Normals_ID = items[2];
		UV_ID = items[3];
		Size = items[4];
		Hitbox[0].x = *reinterpret_cast<float *>(&items[5]);
		Hitbox[0].y = *reinterpret_cast<float *>(&items[6]);
		Hitbox[0].z = *reinterpret_cast<float *>(&items[7]);
		Hitbox[1].x = *reinterpret_cast<float *>(&items[8]);
		Hitbox[1].y = *reinterpret_cast<float *>(&items[9]);
		Hitbox[1].z = *reinterpret_cast<float *>(&items[10]);
		delete Vertex_References;
		Vertex_References = reinterpret_cast<unsigned short *>(items[11]);
		*Vertex_References += 1; // Essentially a copy of the vertices
	}
	else
	{

		std::ifstream file(filename, std::ios::in);
		if (!file)
		{
			std::cout << "Unable to open file " << filename << ". Replacing mesh with empty mesh." << std::endl;
			return;
		}

		std::string line;
		std::vector<glm::vec3> temp_vertices;
		std::vector<glm::vec2> temp_textures;
		std::vector<glm::vec3> temp_normals;
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> textures;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> bump_x_normals;
		std::vector<glm::vec3> bump_y_normals;

		while (std::getline(file, line))
		{
			if (line[0] == 'v' && line[1] == ' ')
			{
				line = line.substr(2);
				const char * stream = line.c_str();
				glm::vec3 option;
				int next;
				option.x = Indigo::Fast_Float(stream, &next);
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
				option.x = Indigo::Fast_Float(stream, &next);
				option.y = 1 - Indigo::Fast_Float(stream, nullptr, next); // For some reason all UVs are Upside-Down!
				temp_textures.push_back(option);
			}
			if (line[0] == 'v' && line[1] == 'n')
			{
				line = line.substr(3);
				const char * stream = line.c_str();
				glm::vec3 normal;
				int next;
				normal.x = Indigo::Fast_Float(stream, &next);
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
							textures.push_back(temp_textures[atoi(values.c_str()) - 1]);
						}
						else
						{
							textures.push_back(glm::vec2(0, 0));
						}
						// Vertex Normal
						values = values.substr(values.find('/') + 1);
						if (values[0] != ' ')
						{
							int place = atoi(values.c_str()) - 1;
							if (temp_normals.size() > place)
							{
								normals.push_back(temp_normals[place]);
							}
							else
							{
								if (point == 2)
								{
									int start = vertices.size() - 1;
									glm::vec3 flat_normal = glm::cross(vertices[start - 1] - vertices[start - 2], vertices[start] - vertices[start - 2]);
									normals.push_back(flat_normal);
									normals.push_back(flat_normal);
									normals.push_back(flat_normal);
								}
							}
						}
						if (point == 2)
						{
							int start = vertices.size() - 1;
							glm::vec2 uv_dir_1 = textures[start] - textures[start - 1];
							glm::vec2 uv_dir_2 = textures[start] - textures[start - 2];
							glm::vec3 vert_dir_1 = vertices[start] - vertices[start - 1];
							glm::vec3 vert_dir_2 = vertices[start] - vertices[start - 2];
							float denominator = 1.0 / (uv_dir_1.x * uv_dir_2.y - uv_dir_1.y * uv_dir_2.x);
							glm::vec3 bump_x_add = (vert_dir_1 * uv_dir_2.y - vert_dir_2 * uv_dir_1.y) * denominator;
							glm::vec3 bump_y_add = (vert_dir_2 * uv_dir_1.x - vert_dir_1 * uv_dir_2.x) * denominator;
							bump_x_normals.push_back(bump_x_add);
							bump_x_normals.push_back(bump_x_add);
							bump_x_normals.push_back(bump_x_add);
							bump_y_normals.push_back(bump_y_add);
							bump_y_normals.push_back(bump_y_add);
							bump_y_normals.push_back(bump_y_add);
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

		Initialize(vertices, textures, normals, bump_x_normals, bump_y_normals);

		std::vector<unsigned int> items;
		items.push_back(Vertices_ID);
		items.push_back(Elements_ID);
		items.push_back(Normals_ID);
		items.push_back(UV_ID);
		items.push_back(Size);
		items.push_back(*reinterpret_cast<unsigned int *>(&Hitbox[0].x));
		items.push_back(*reinterpret_cast<unsigned int *>(&Hitbox[0].y));
		items.push_back(*reinterpret_cast<unsigned int *>(&Hitbox[0].z));
		items.push_back(*reinterpret_cast<unsigned int *>(&Hitbox[1].x));
		items.push_back(*reinterpret_cast<unsigned int *>(&Hitbox[1].y));
		items.push_back(*reinterpret_cast<unsigned int *>(&Hitbox[1].z));
		items.push_back(reinterpret_cast<unsigned int>(Vertex_References));
		Load_Once[Obj_File_Hash] = items;

	}

	if (texture == nullptr)
	{
		Texture();
		return;
	}

	Texture(texture);

	return;

}


// Specialized constructor for creating text
Mesh Mesh::Text(const char * text, const float size, const char * font, const glm::vec4& highlight)
{
	Mesh mesh;
	float bottom = 0;
	float top = size;
	float left = 0;
	float right = size;
	int i = 0;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> bump_x;
	std::vector<glm::vec3> bump_y;
	for (char check_letter = text[0]; check_letter != '\0'; check_letter = text[++i])
	{

		int letter = check_letter - 32;
		float y = (letter / 16) / 16.0; // The most 16s you can get
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
		float tex_b = y + 0.0625;
		uvs.push_back(glm::vec2(tex_l, tex_b)); // BL
		uvs.push_back(glm::vec2(tex_r, tex_b)); // BR
		uvs.push_back(glm::vec2(tex_r, tex_t)); // TR
		uvs.push_back(glm::vec2(tex_l, tex_b)); // BL
		uvs.push_back(glm::vec2(tex_r, tex_t)); // TR
		uvs.push_back(glm::vec2(tex_l, tex_t)); // TL

		normals.push_back(glm::vec3(0, 0, 0));
		normals.push_back(glm::vec3(0, 0, 0));
		normals.push_back(glm::vec3(0, 0, 0));
		normals.push_back(glm::vec3(0, 0, 0));
		normals.push_back(glm::vec3(0, 0, 0));
		normals.push_back(glm::vec3(0, 0, 0));

		bump_x.push_back(glm::vec3(0, 0, 0));
		bump_x.push_back(glm::vec3(0, 0, 0));
		bump_x.push_back(glm::vec3(0, 0, 0));
		bump_x.push_back(glm::vec3(0, 0, 0));
		bump_x.push_back(glm::vec3(0, 0, 0));
		bump_x.push_back(glm::vec3(0, 0, 0));

		bump_y.push_back(glm::vec3(0, 0, 0));
		bump_y.push_back(glm::vec3(0, 0, 0));
		bump_y.push_back(glm::vec3(0, 0, 0));
		bump_y.push_back(glm::vec3(0, 0, 0));
		bump_y.push_back(glm::vec3(0, 0, 0));
		bump_y.push_back(glm::vec3(0, 0, 0));

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
	mesh.Initialize(vertices, uvs, normals, bump_x, bump_y, false);
	if (highlight.a == 0)
	{
		mesh.Texture(font);
	}
	else
	{
		mesh.Texture(font, glm::vec3(highlight.r, highlight.g, highlight.b));
	}
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
void Mesh::Initialize(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& uvs, const std::vector<glm::vec3>& normals,
	const std::vector<glm::vec3>& bump_x_normals, const std::vector<glm::vec3>& bump_y_normals, const bool memory_optimize)
{

	// Index for the VBO!
	std::vector<glm::vec3> final_vertices = std::vector<glm::vec3>();
	std::vector<glm::vec2> final_uvs = std::vector<glm::vec2>();
	std::vector<glm::vec3> final_normals = std::vector<glm::vec3>();
	std::vector<glm::vec3> final_bump_x = std::vector<glm::vec3>();
	std::vector<glm::vec3> final_bump_y = std::vector<glm::vec3>();
	std::vector<unsigned short> elements = std::vector<unsigned short>();
	if (memory_optimize)
	{
		std::map<Vertex_Texture_Normal, unsigned short> vertex_to_index;
		for (int point = 0; point < vertices.size(); ++point)
		{
			Vertex_Texture_Normal together = { vertices[point], uvs[point], normals[point] };
			std::map<Vertex_Texture_Normal, unsigned short>::iterator location = vertex_to_index.find(together);
			if (location == vertex_to_index.end())
			{
				final_vertices.push_back(vertices[point]);
				final_uvs.push_back(uvs[point]);
				final_normals.push_back(normals[point]);
				final_bump_x.push_back(bump_x_normals[point]);
				final_bump_y.push_back(bump_y_normals[point]);
				Update_Hitbox(vertices[point]);
				unsigned short index = final_vertices.size() - 1;
				elements.push_back(index);
				vertex_to_index[together] = index;
			}
			else
			{
				final_bump_x[location->second] += bump_x_normals[point];
				final_bump_y[location->second] += bump_y_normals[point];
				elements.push_back(location->second);
			}
		}
	}
	else
	{
		for (int point = 0; point < vertices.size(); ++point)
		{
			final_vertices.push_back(vertices[point]);
			final_uvs.push_back(uvs[point]);
			final_normals.push_back(normals[point]);
			final_bump_x.push_back(bump_x_normals[point]);
			final_bump_y.push_back(bump_y_normals[point]);
			Update_Hitbox(vertices[point]);
			elements.push_back(point);
		}
	}

	// Let's do it! It's ready! Let's send it to the GPU!
	glGenBuffers(1, &Vertices_ID);
	glBindBuffer(GL_ARRAY_BUFFER, Vertices_ID);
	glBufferData(GL_ARRAY_BUFFER, final_vertices.size() * sizeof(glm::vec3), &final_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &Normals_ID);
	glBindBuffer(GL_ARRAY_BUFFER, Normals_ID);
	glBufferData(GL_ARRAY_BUFFER, final_normals.size() * sizeof(glm::vec3), &final_normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &Bump_X_Normals_ID);
	glBindBuffer(GL_ARRAY_BUFFER, Bump_X_Normals_ID);
	glBufferData(GL_ARRAY_BUFFER, final_bump_x.size() * sizeof(glm::vec3), &final_bump_x[0], GL_STATIC_DRAW);

	glGenBuffers(1, &Bump_Y_Normals_ID);
	glBindBuffer(GL_ARRAY_BUFFER, Bump_Y_Normals_ID);
	glBufferData(GL_ARRAY_BUFFER, final_bump_y.size() * sizeof(glm::vec3), &final_bump_y[0], GL_STATIC_DRAW);

	glGenBuffers(1, &UV_ID);
	glBindBuffer(GL_ARRAY_BUFFER, UV_ID);
	glBufferData(GL_ARRAY_BUFFER, final_uvs.size() * sizeof(glm::vec2), &final_uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &Elements_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Elements_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned short), &elements[0], GL_STATIC_DRAW);

	// Oh, yeah, we actually need to render this. How big was this model, again?
	Size = elements.size();

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


void Mesh::Initialize_Texture(unsigned int& handle, const unsigned char * data, const int width, const int height, const int channels)
{
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, (channels == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
}


// Texture the entire mesh with one file
void Mesh::Texture(const char * filename, const glm::vec3 background)
{

	if (filename)
	{
		for (int i = 0; filename[i] != '\0'; ++i)
		{
			Texture_File_Hash += int(filename[i]);
			Texture_File_Hash %= 4294967291; // Largest prime < 2^32
		}
		if (background != glm::vec3(-1, -1, -1))
		{
			Texture_File_Hash += 351; // Different hash if we're modifying the source file. (We're highlighting)
		}
	}
	else
	{
		Texture_File_Hash = 2304927; // Some random fun number for blank texture's handle hash
	}
	std::map<unsigned int, std::vector<unsigned int>>::iterator location = Load_Once.find(Texture_File_Hash);
	if (location != Load_Once.end() && location->second.size() > 1)
	{
		std::vector<unsigned int>& items = location->second;
		Texture_ID = items[0];
		Texture_References = (unsigned short *) items[1];
		*Texture_References += 1; // Essentially a copy of a texture
	}
	else
	{
		unsigned char * data;
		int width;
		int height;
		int channels = 3;
		bool std_free = false;
		if (!filename)
		{
			data = new unsigned char[3];
			data[0] = 255;
			data[1] = 255;
			data[2] = 255;
			width = 1;
			height = 1;
			std_free = true;
		}
		else
		{
			data = stbi_load(filename, &width, &height, &channels, 0);
			if (channels < 3)
			{
				std::cout << "Unable to load texture " << filename << ": " << stbi_failure_reason << ". Failing silently with White texture." << std::endl;
				stbi_image_free(data);
				Texture();
				return;
			}
			if (background != glm::vec3(-1, -1, -1) && channels == 4)
			{
				channels = 3;
				unsigned char * bged = new unsigned char[width*height * 3];
				int insert = 0;
				for (int i = 3; i < width*height * 4; i += 4)
				{
					if (data[i] < 128)
					{
						bged[insert] = (unsigned char)(background.r * 255);
						bged[insert + 1] = (unsigned char)(background.g * 255);
						bged[insert + 2] = (unsigned char)(background.b * 255);
					}
					else
					{
						bged[insert] = data[i - 3];
						bged[insert + 1] = data[i - 2];
						bged[insert + 2] = data[i - 1];
					}
					insert += 3;
				}
				stbi_image_free(data);
				data = bged;
				std_free = true;
			}
		}

		Initialize_Texture(Texture_ID, data, width, height, channels);

		if (std_free)
		{
			delete[] data;
		}
		else
		{
			stbi_image_free(data);
		}
		std::vector<unsigned int> id;
		id.push_back(Texture_ID);
		id.push_back((unsigned int) Texture_References);
		Load_Once[Texture_File_Hash] = id;
	}

	return;
}


// A special texture that adds normals to each pixel. Default blue.
void Mesh::Bump_Map(const char * filename)
{

	if (filename)
	{
		for (int i = 0; filename[i] != '\0'; ++i)
		{
			Texture_File_Hash += int(filename[i]);
			Texture_File_Hash %= 4294967291; // Largest prime < 2^32
		}
	}
	else
	{
		Texture_File_Hash = 2304927; // Some random fun number for blank texture's handle hash
	}
	std::map<unsigned int, std::vector<unsigned int>>::iterator location = Load_Once.find(Texture_File_Hash);
	if (location != Load_Once.end() && location->second.size() > 1)
	{
		std::vector<unsigned int>& items = location->second;
		Bump_Texture_ID = items[0];
		Texture_References = (unsigned short *) items[1];
		*Texture_References += 1; // Essentially a copy of a texture
	}
	else
	{
		unsigned char * data;
		int width;
		int height;
		int channels = 3;
		bool std_free = false;
		if (!filename)
		{
			data = new unsigned char[3];
			data[0] = 0;
			data[1] = 0;
			data[2] = 255;
			width = 1;
			height = 1;
			std_free = true;
		}
		else
		{
			data = stbi_load(filename, &width, &height, &channels, 0);
			if (channels < 3)
			{
				std::cout << "Unable to load texture " << filename << ": " << stbi_failure_reason << ". Failing silently with White texture." << std::endl;
				stbi_image_free(data);
				Texture();
				return;
			}
		}

		Initialize_Texture(Bump_Texture_ID, data, width, height, channels);

		if (std_free)
		{
			delete[] data;
		}
		else
		{
			stbi_image_free(data);
		}
		std::vector<unsigned int> id;
		id.push_back(Bump_Texture_ID);
		id.push_back((unsigned int) Texture_References);
		Load_Once[Texture_File_Hash] = id;
	}
}



std::map<unsigned int, std::vector<unsigned int>> Mesh::Load_Once = std::map<unsigned int, std::vector<unsigned int>>();
