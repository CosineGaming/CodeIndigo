// Defines a mesh of points for render

#include "Mesh.h"

#include "Direction.h"
#include "Indigo.h"
#include "External/stb_image.c"

#include <fstream>
#include <iostream>
#include "GLFW/glfw3.h"

#define Hash_Modulo 4294967291 // Largest Prime < 2^32


// When we're done we have to delete the image (done by Initialize_Texture automatically)
void Texture_Data::Free(void)
{
	if (Width == 0 && Height == 0)
	{
		// Never allocated, nothing to do
	}
	else if (Width == 1 && Height == 1)
	{
		// C++ allocation
		delete[] Image;
	}
	else
	{
		// STBI allocation
		stbi_image_free(Image);
	}
	// To prevent double frees, we set it to "never allocated"
	Width = 0;
	Height = 0;
}


// Create a new, empty mesh
Mesh::Mesh(void) :
	Vertices_ID(0),
	Elements_ID(0),
	Normals_ID(0),
	Bump_X_Normals_ID(0),
	Bump_Y_Normals_ID(0),
	UV_ID(0),
	Texture_ID(0),
	Bump_Texture_ID(0),
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
	Bump_Texture_ID(mesh.Bump_Texture_ID),
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


// Assign a mesh. Resource management - Needs to be wibbled and yall
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
		Bump_Texture_ID = mesh.Bump_Texture_ID;
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
		if (Bump_Texture_ID)
		{
			glDeleteTextures(1, &Bump_Texture_ID);
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
Mesh::Mesh(const char * filename, const char * texture, const char * bump_map) :
	Mesh()
{

	for (int i = 0; filename[i] != '\0'; ++i)
	{
		Obj_File_Hash += int(filename[i]);
		Obj_File_Hash %= Hash_Modulo; // Largest prime < 2^32
	}
	std::map<unsigned int, std::vector<unsigned int>>::iterator location = Load_Once.find(Obj_File_Hash);
	if (location != Load_Once.end() && location->second.size())
	{
		std::vector<unsigned int>& items = location->second;
		Vertices_ID = items[0];
		Elements_ID = items[1];
		Normals_ID = items[2];
		Bump_X_Normals_ID = items[3];
		Bump_Y_Normals_ID = items[4];
		UV_ID = items[5];
		Size = items[6];
		Hitbox[0].x = *reinterpret_cast<float *>(&items[7]);
		Hitbox[0].y = *reinterpret_cast<float *>(&items[8]);
		Hitbox[0].z = *reinterpret_cast<float *>(&items[9]);
		Hitbox[1].x = *reinterpret_cast<float *>(&items[10]);
		Hitbox[1].y = *reinterpret_cast<float *>(&items[11]);
		Hitbox[1].z = *reinterpret_cast<float *>(&items[12]);
		delete Vertex_References;
		Vertex_References = reinterpret_cast<unsigned short *>(items[13]);
		*Vertex_References += 1; // Essentially a copy of the vertices
	}
	else
	{

		Initialize_Vertices(File_Vertices(filename));

		std::vector<unsigned int> items;
		items.push_back(Vertices_ID);
		items.push_back(Elements_ID);
		items.push_back(Normals_ID);
		items.push_back(Bump_X_Normals_ID);
		items.push_back(Bump_Y_Normals_ID);
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

	Texture(texture);

	Bump_Map(bump_map);

	return;

}


// Specialized constructor for creating text
Mesh Mesh::Text(const char * text, const float size, const char * font, const glm::vec4& text_color, const glm::vec4& highlight, const int reduce_filter, const int enlarge_filter)
{

	Mesh mesh;
	Vertex_Data data = Text_Vertices(text, size);
	mesh.Initialize_Vertices(data, false);

	// Modified version of Texture() inserted here for highlighting
	if (font)
	{
		for (int i = 0; font[i] != '\0'; ++i)
		{
			mesh.Texture_File_Hash += int(font[i]);
			mesh.Texture_File_Hash %= Hash_Modulo; // Largest prime < 2^32
		}
		// If an image was passed with different filters, we unfortunately need to load it twice with different parameters
		mesh.Texture_File_Hash += reduce_filter * 2; // If we switch the filters, it should still change. So we multiply times two to seperate them
		mesh.Texture_File_Hash %= Hash_Modulo;
		mesh.Texture_File_Hash += enlarge_filter;
		mesh.Texture_File_Hash %= Hash_Modulo;
		mesh.Texture_File_Hash += glm::dot(text_color, glm::vec4(25, 25, 25, 25)); // Value up to 100 based on text color
		mesh.Texture_File_Hash %= Hash_Modulo;
		mesh.Texture_File_Hash += glm::dot(highlight, glm::vec4(25, 25, 25, 25)); // Value based on highlight
		mesh.Texture_File_Hash %= Hash_Modulo;
	}
	else
	{
		// Some random fun number for blank texture's handle hash. You can't sample a 1x1 so filter's aren't used
		mesh.Texture_File_Hash = 2304927;
	}
	std::map<unsigned int, std::vector<unsigned int>>::iterator location = Load_Once.find(mesh.Texture_File_Hash);
	if (location != Load_Once.end() && location->second.size() > 1)
	{
		// Already been loaded
		std::vector<unsigned int>& items = location->second;
		mesh.Texture_ID = items[0];
		mesh.Texture_References = (unsigned short *) items[1];
		*mesh.Texture_References += 1; // Essentially a copy of a texture
	}
	else
	{
		// Need to load it
		Texture_Data texture = Load_Image(font);
		texture.Reduce_Filter = reduce_filter;
		texture.Enlarge_Filter = enlarge_filter;

		// Here's where we color the text if required
		if (texture.Channels == 4 && (highlight.a != 0 || text_color != glm::vec4(1, 1, 1, 1)))
		{
			for (int i = 0; i < texture.Width * texture.Height * texture.Channels; i += texture.Channels) // Loop through each pixel
			{
				if (texture.Image[i + 3] < 32) // Alpha is esentially zero (Universal photoshop "close enough" is 32)
				{
					// Replace it (passed as 0..1 used as 0..255)
					if (highlight.a != 0)
					{
						texture.Image[i] = highlight.r * 255;
						texture.Image[i + 1] = highlight.g * 255;
						texture.Image[i + 2] = highlight.b * 255;
						texture.Image[i + 3] = highlight.a * 255;
					}
				}
				else
				{
					// Color the text
					if (text_color != glm::vec4(1, 1, 1, 1))
					{
						texture.Image[i] = text_color.r * 255;
						texture.Image[i + 1] = text_color.g * 255;
						texture.Image[i + 2] = text_color.b * 255;
						texture.Image[i + 3] = text_color.a * 255;
					}
				}
			}
		}

		mesh.Initialize_Texture(texture, &mesh.Texture_ID);

		// Remember we loaded it
		std::vector<unsigned int> id;
		id.push_back(mesh.Texture_ID);
		id.push_back((unsigned int) mesh.Texture_References);
		Load_Once[mesh.Texture_File_Hash] = id;
	}

	mesh.Bump_Map();

	return mesh;

}


// Specialized constructor for creating 2D flat rectangles
Mesh Mesh::Rectangle(const float width, const float height, const char * texture, const char * bump_map, const int reduce_filter, const int enlarge_filter)
{
	Mesh mesh;
	mesh.Texture(texture);
	mesh.Bump_Map(bump_map);
	Vertex_Data data = Rectangle_Vertices(width, height);
	mesh.Initialize_Vertices(data);
	return mesh;
}


// Returns CPU Mesh_Data of a Mesh from file
Vertex_Data Mesh::File_Vertices(const char * filename)
{

	Vertex_Data data;

	std::ifstream file(filename, std::ios::in);
	if (!file)
	{
		std::cout << "Unable to open file " << filename << ". Replacing mesh with empty mesh." << std::endl;
		return data;
	}

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
				data.Positions.push_back(temp_vertices[atoi(values.c_str()) - 1]);
				// Vertex Texture
				int texturenormal = values.find('/') + 1;
				if (texturenormal != std::string::npos)
				{
					values = values.substr(texturenormal);
					if (values[0] != '/')
					{
						data.UVs.push_back(temp_textures[atoi(values.c_str()) - 1]);
					}
					else
					{
						data.UVs.push_back(glm::vec2(0, 0));
					}
					// Vertex Normal
					values = values.substr(values.find('/') + 1);
					if (values[0] != ' ')
					{
						int place = atoi(values.c_str()) - 1;
						if (temp_normals.size() > place)
						{
							data.Normals.push_back(temp_normals[place]);
						}
						else
						{
							if (point == 2)
							{
								int start = data.Positions.size() - 1;
								glm::vec3 flat_normal = glm::cross(data.Positions[start - 1] - data.Positions[start - 2], data.Positions[start] - data.Positions[start - 2]);
								data.Normals.push_back(flat_normal);
								data.Normals.push_back(flat_normal);
								data.Normals.push_back(flat_normal);
							}
						}
					}
					if (point == 2)
					{
						int start = data.Positions.size() - 1;
						glm::vec2 uv_dir_1 = data.UVs[start - 1] - data.UVs[start - 2];
						glm::vec2 uv_dir_2 = data.UVs[start] - data.UVs[start - 2];
						glm::vec3 vert_dir_1 = data.Positions[start - 1] - data.Positions[start - 2];
						glm::vec3 vert_dir_2 = data.Positions[start] - data.Positions[start - 2];
						float denominator = 1.0 / (uv_dir_1.x * uv_dir_2.y - uv_dir_1.y * uv_dir_2.x);
						glm::vec3 bump_x_add = (vert_dir_1 * uv_dir_2.y - vert_dir_2 * uv_dir_1.y) * denominator;
						bump_x_add = bump_x_add - data.Normals[start] * glm::dot(data.Normals[start], bump_x_add);
						glm::vec3 bump_y_add = (vert_dir_2 * uv_dir_1.x - vert_dir_1 * uv_dir_2.x) * denominator;
						data.Bump_X_Normals.push_back(bump_x_add);
						data.Bump_X_Normals.push_back(bump_x_add);
						data.Bump_X_Normals.push_back(bump_x_add);
						data.Bump_Y_Normals.push_back(bump_y_add);
						data.Bump_Y_Normals.push_back(bump_y_add);
						data.Bump_Y_Normals.push_back(bump_y_add);
					}
				}
				else
				{
					data.UVs.push_back(glm::vec2(0, 0));
					data.Normals.push_back(glm::cross(data.Positions[point - 1] - data.Positions[point - 2], data.Positions[point] - data.Positions[point - 2]));
				}
				// Move on to next point
				values = values.substr(values.find(' ') + 1);
			}
		}
	}

	file.close();

	return data;

}


// Returns CPU Vertex_Data of a Text
Vertex_Data Mesh::Text_Vertices(const char * text, const float size)
{

	float bottom = 0;
	float top = size;
	float left = 0;
	float right = size;
	int i = 0;
	Vertex_Data mesh;
	for (char check_letter = text[0]; check_letter != '\0'; check_letter = text[++i])
	{

		int letter = check_letter - 32;
		float y = (letter / 16) / 16.0; // The most 16s you can get
		float x = (letter % 16) / 16.0; // The rest
		mesh.Positions.push_back(glm::vec3(left, bottom, 0)); // BL
		mesh.Positions.push_back(glm::vec3(right, bottom, 0)); // BR
		mesh.Positions.push_back(glm::vec3(right, top, 0)); // TR
		mesh.Positions.push_back(glm::vec3(left, bottom, 0)); // BL
		mesh.Positions.push_back(glm::vec3(right, top, 0)); // TR
		mesh.Positions.push_back(glm::vec3(left, top, 0)); // TL

		float tex_l = x;
		float tex_r = x + 0.0625;
		float tex_t = y;
		float tex_b = y + 0.0625;
		mesh.UVs.push_back(glm::vec2(tex_l, tex_b)); // BL
		mesh.UVs.push_back(glm::vec2(tex_r, tex_b)); // BR
		mesh.UVs.push_back(glm::vec2(tex_r, tex_t)); // TR
		mesh.UVs.push_back(glm::vec2(tex_l, tex_b)); // BL
		mesh.UVs.push_back(glm::vec2(tex_r, tex_t)); // TR
		mesh.UVs.push_back(glm::vec2(tex_l, tex_t)); // TL

		for (int trash = 0; trash < 6; ++trash)
		{
			mesh.Normals.push_back(glm::vec3(0, 0, 0));
			mesh.Bump_X_Normals.push_back(glm::vec3(0, 0, 0));
			mesh.Bump_Y_Normals.push_back(glm::vec3(0, 0, 0));
		}

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

	return mesh;

}


// Returns CPU Mesh_Data of a Rectangle
Vertex_Data Mesh::Rectangle_Vertices(const float width, const float height)
{

	float hw = width / 2;
	float hh = height / 2;
	// Texture coordinates shouldn't stretch. Texture is in top-left
	float wth = width / height;
	float htw = 1 / wth;
	if (wth > 1)
		wth = 1;
	if (htw > 1)
		htw = 1;
	Vertex_Data data;
	data.Positions.push_back(glm::vec3(-hw, -hh, 0));
	data.Positions.push_back(glm::vec3(hw, -hh, 0));
	data.Positions.push_back(glm::vec3(hw, hh, 0));
	data.Positions.push_back(glm::vec3(-hw, -hh, 0));
	data.Positions.push_back(glm::vec3(hw, hh, 0));
	data.Positions.push_back(glm::vec3(-hw, hh, 0));
	data.UVs.push_back(glm::vec2(0, 0));
	data.UVs.push_back(glm::vec2(wth, 0));
	data.UVs.push_back(glm::vec2(wth, htw));
	data.UVs.push_back(glm::vec2(0, 0));
	data.UVs.push_back(glm::vec2(wth, htw));
	data.UVs.push_back(glm::vec2(0, htw));
	for (int i = 0; i < 6; ++i)
	{
		// Same lighting for each vertex: out
		data.Bump_X_Normals.push_back(glm::vec3(1, 0, 0));
		data.Bump_Y_Normals.push_back(glm::vec3(0, 1, 0));
		data.Normals.push_back(glm::vec3(0, 0, 1));
	}
	return data;

}


// Used by Initialize's maps
struct Compared_Vertex
{
	glm::vec3 vertex;
	glm::vec2 texture;
	glm::vec3 normal;
	bool operator< (const Compared_Vertex& compare) const
	{
		return memcmp(this, &compare, sizeof(Compared_Vertex)) > 0;
	}
};


// Once added to the object, the mesh is locked into place. (on the GPU)
void Mesh::Initialize_Vertices(const Vertex_Data& initialize, const bool memory_optimize)
{

	// Index for the VBO
	const Vertex_Data * data;
	Vertex_Data output;
	std::vector<unsigned short> elements = std::vector<unsigned short>();
	if (memory_optimize)
	{
		std::map<Compared_Vertex, unsigned short> vertex_to_index;
		for (int point = 0; point < initialize.Positions.size(); ++point)
		{
			Compared_Vertex together = { initialize.Positions[point], initialize.UVs[point], initialize.Normals[point] };
			std::map<Compared_Vertex, unsigned short>::iterator location = vertex_to_index.find(together);
			if (location == vertex_to_index.end())
			{
				output.Positions.push_back(initialize.Positions[point]);
				output.UVs.push_back(initialize.UVs[point]);
				output.Normals.push_back(initialize.Normals[point]);
				output.Bump_X_Normals.push_back(initialize.Bump_X_Normals[point]);
				output.Bump_Y_Normals.push_back(initialize.Bump_Y_Normals[point]);
				Update_Hitbox(initialize.Positions[point]);
				unsigned short index = output.Positions.size() - 1;
				elements.push_back(index);
				vertex_to_index[together] = index;
			}
			else
			{
				output.Bump_X_Normals[location->second] += initialize.Bump_X_Normals[point];
				output.Bump_Y_Normals[location->second] += initialize.Bump_Y_Normals[point];
				elements.push_back(location->second);
			}
		}
		data = &output;
	}
	else
	{
		data = &initialize;
		for (int point = 0; point < initialize.Positions.size(); ++point)
		{
			elements.push_back(point);
			Update_Hitbox(initialize.Positions[point]);
		}
	}

	// Let's do it! It's ready! Let's send it to the GPU!
	glGenBuffers(1, &Vertices_ID);
	glBindBuffer(GL_ARRAY_BUFFER, Vertices_ID);
	glBufferData(GL_ARRAY_BUFFER, data->Positions.size() * sizeof(glm::vec3), &data->Positions[0], GL_STATIC_DRAW);

	glGenBuffers(1, &Normals_ID);
	glBindBuffer(GL_ARRAY_BUFFER, Normals_ID);
	glBufferData(GL_ARRAY_BUFFER, data->Normals.size() * sizeof(glm::vec3), &data->Normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &Bump_X_Normals_ID);
	glBindBuffer(GL_ARRAY_BUFFER, Bump_X_Normals_ID);
	glBufferData(GL_ARRAY_BUFFER, data->Bump_X_Normals.size() * sizeof(glm::vec3), &data->Bump_X_Normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &Bump_Y_Normals_ID);
	glBindBuffer(GL_ARRAY_BUFFER, Bump_Y_Normals_ID);
	glBufferData(GL_ARRAY_BUFFER, data->Bump_Y_Normals.size() * sizeof(glm::vec3), &data->Bump_Y_Normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &UV_ID);
	glBindBuffer(GL_ARRAY_BUFFER, UV_ID);
	glBufferData(GL_ARRAY_BUFFER, data->UVs.size() * sizeof(glm::vec2), &data->UVs[0], GL_STATIC_DRAW);

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
		Hitbox[0].x = vertex.x;
	if (vertex.x > Hitbox[1].x)
		Hitbox[1].x = vertex.x;
	if (vertex.y < Hitbox[0].y)
		Hitbox[0].y = vertex.y;
	if (vertex.y > Hitbox[1].y)
		Hitbox[1].y = vertex.y;
	if (vertex.z < Hitbox[0].z)
		Hitbox[0].z = vertex.z;
	if (vertex.z > Hitbox[1].z)
		Hitbox[1].z = vertex.z;
	return;
}


// Load the texture and initialize it to the handle, if it's not a duplicate
void Mesh::Texture(const char * filename, unsigned int * texture_handle, const int reduce_filter, const int enlarge_filter)
{

	if (texture_handle == nullptr)
	{
		texture_handle = &Texture_ID;
	}

	if (filename)
	{
		for (int i = 0; filename[i] != '\0'; ++i)
		{
			Texture_File_Hash += int(filename[i]);
			Texture_File_Hash %= Hash_Modulo; // Largest prime < 2^32
		}
		// If an image was passed with different filters, we unfortunately need to load it twice with different parameters
		Texture_File_Hash += reduce_filter * 2; // If we switch the filters, it should still change. So we multiply times two to seperate them
		Texture_File_Hash %= Hash_Modulo;
		Texture_File_Hash += enlarge_filter;
		Texture_File_Hash %= Hash_Modulo;
	}
	else
	{
		// Some random fun number for blank texture's handle hash. You can't sample a 1x1 so filter's aren't used
		Texture_File_Hash = 2304927;
		// A blank bump is different from a blank non-bump
		Texture_File_Hash += texture_handle == &Bump_Texture_ID;
	}
	std::map<unsigned int, std::vector<unsigned int>>::iterator location = Load_Once.find(Texture_File_Hash);
	if (location != Load_Once.end() && location->second.size() > 1)
	{
		// Already been loaded
		std::vector<unsigned int>& items = location->second;
		*texture_handle = items[0];
		Texture_References = (unsigned short *) items[1];
		*Texture_References += 1; // Essentially a copy of a texture
	}
	else
	{
		// Need to load it
		Texture_Data texture = Load_Image(filename);
		texture.Reduce_Filter = reduce_filter;
		texture.Enlarge_Filter = enlarge_filter;
		if (texture_handle == &Bump_Texture_ID)
		{
			// By default, bump texture should be completely blue
			if (texture.Width == 1 && texture.Height == 1)
			{
				texture.Image[0] = 0;
				texture.Image[1] = 0;
				texture.Image[2] = 255; // rgb: Blue
			}
		}
		Initialize_Texture(texture, texture_handle);

		// Remember we loaded it
		std::vector<unsigned int> id;
		id.push_back(*texture_handle);
		id.push_back((unsigned int) Texture_References);
		Load_Once[Texture_File_Hash] = id;
	}

	return;

}


// Simply calls Texture with texture_handle = &Bump_Texture_ID
void Mesh::Bump_Map(const char * filename, const int reduce_filter, const int enlarge_filter)
{
	Texture(filename, &Bump_Texture_ID, reduce_filter, enlarge_filter);
}


// Initialize only the texture data, for animations which use the same mesh for example
unsigned int Mesh::Initialize_Texture(Texture_Data& initialize, unsigned int * texture_handle)
{

	if (!texture_handle)
	{
		texture_handle = &Texture_ID;
	}
	glGenTextures(1, texture_handle);
	glBindTexture(GL_TEXTURE_2D, *texture_handle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, initialize.Width, initialize.Height, 0, (initialize.Channels == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, initialize.Image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, initialize.Reduce_Filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, initialize.Enlarge_Filter);
	initialize.Free();
	glGenerateMipmap(GL_TEXTURE_2D);
	return *texture_handle;

}


// Get the data from an image and return it
Texture_Data Mesh::Load_Image(const char * filename)
{

	Texture_Data texture;
	if (!filename)
	{
		texture.Image = new unsigned char[3];
		texture.Image[0] = 255;
		texture.Image[1] = 255;
		texture.Image[2] = 255;
		texture.Width = 1;
		texture.Height = 1;
	}
	else
	{
		texture.Image = stbi_load(filename, &texture.Width, &texture.Height, &texture.Channels, 0);
		if (texture.Channels < 3)
		{
			std::cout << "Unable to load texture " << filename << ": " << stbi_failure_reason << ". Failing silently with White texture." << std::endl;
			return Load_Image();
		}
	}
	return texture;

}


// Stores a hash of the filename mapped to the handle, so that the same file isn't loaded twice
std::map<unsigned int, std::vector<unsigned int>> Mesh::Load_Once = std::map<unsigned int, std::vector<unsigned int>>();
