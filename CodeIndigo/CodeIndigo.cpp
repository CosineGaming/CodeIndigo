#include "Indigo/IndigoEngine.h"
#include <iostream>
#include <fstream>
#include <map>
#include <string>


struct Mesh_Texture
{
	std::string mesh;
	std::string texture;
	bool operator<(const Mesh_Texture& compare) const
	{
		return memcmp(this, &compare, sizeof(Mesh_Texture)) > 0;
	}
};

World restore;

Object Motion;
Object * To_Move = &Motion;

std::map<Mesh_Texture, unsigned short> model_to_index;
std::vector<Mesh_Texture> models;
std::vector<unsigned short> instances;
std::vector<glm::vec3> positions;

bool Typing;
bool Snap;

int Position_Marker = -1;

bool Load(const char * filename)
{
	/* File Format:
	// Comments are usual style (/ works also). Meshes:
	mesh|texture
	> Anything can be here but must start with this character.
	instance|x|y|z
	> Globals:
	camerax|y|z
	*/
	std::ifstream file(filename);
	if (!file)
	{
		return false;
	}
	std::string line;
	int state = 0;
	std::vector<Mesh> meshes;
	while (std::getline(file, line))
	{
		if (line[0] == '>')
		{
			state++;
			continue;
		}
		if (line[0] == '/')
		{
			continue;
		}
		if (state == 0)
		{
			int separation = line.find('|');
			std::string mesh = line.substr(0, separation);
			std::string texture = line.substr(separation + 1);
			Mesh_Texture model = { mesh.c_str(), texture.c_str() };
			meshes.push_back(Mesh(model.mesh.c_str(), model.texture.c_str()));
			models.push_back(model);
			model_to_index[model] = models.size() - 1;
		}
		if (state == 1)
		{
			int next;
			unsigned short index = Indigo::Fast_Float(line.c_str(), &next);
			float x = Indigo::Fast_Float(line.c_str(), &next, next+1);
			float y = Indigo::Fast_Float(line.c_str(), &next, next+1);
			float z = Indigo::Fast_Float(line.c_str(), nullptr, next+1);
			instances.push_back(index);
			positions.push_back(glm::vec3(x, y, z));
			Indigo::Current_World.Add_Object(Object(x, y, z, meshes[index]));
		}
		if (state == 2)
		{
			int next;
			float x = Indigo::Fast_Float(line.c_str(), &next);
			float y = Indigo::Fast_Float(line.c_str(), &next, next+1);
			float z = Indigo::Fast_Float(line.c_str(), nullptr, next+1);
			Indigo::Current_World.View.Place(x, y, z);
		}
	}
	return true;
}

bool Save(const char * filename)
{
	std::ofstream file(filename);
	if (!file)
	{
		return false;
	}
	file << "// World file for the Indigo Engine" << std::endl << "// Auto-generated by Indigo Engine Level Designer (IELD)." << std::endl;
	file << "// File format:" << std::endl <<
		"// // Comments are usual style (/ works also). Meshes:" << std::endl <<
		"// mesh|texture" << std::endl <<
		"// > Anything can be here but must start with this character." << std::endl <<
		"// instance|x|y|z" << std::endl <<
		"// > Globals:" << std::endl <<
		"// camerax|y|z" << std::endl;
	file << "// Meshes:" << std::endl;
	for (int i = 0; i < models.size(); ++i)
	{
		file << models[i].mesh << "|" << models[i].texture << std::endl;
	}
	file << "> Objects:" << std::endl;
	for (int i = 0; i < instances.size(); ++i)
	{
		file << instances[i] << "|" << positions[i].x << "|" << positions[i].y << "|" << positions[i].z << std::endl;
	}
	file << "> Globals:" << std::endl;
	file << Indigo::Current_World.View.X << "|" << Indigo::Current_World.View.Y << "|" << Indigo::Current_World.View.Z << std::endl;
	return true;
}

bool Save_For_Compile(const char * filename)
{
	std::ofstream file(filename);
	if (!file)
	{
		return false;
	}
	file << "// World file for the Indigo Engine" << std::endl << "// Auto-generated by Indigo Engine Level Designer (IELD)." << std::endl;
	file << "// Actual C++ Indigo Engine code for #including in loading routine." << std::endl;
	file << "// Meshes:" << std::endl;
	for (int i = 0; i < models.size(); ++i)
	{
		file << "Mesh m" << i << " = Mesh(" << models[i].mesh << ", " << models[i].texture << ");" << std::endl;
	}
	file << "// Objects:" << std::endl;
	for (int i = 0; i < instances.size(); ++i)
	{
		file << "Indigo::Current_World.Add_Object(Object(" << positions[i].x << ", " << positions[i].y << ", " << positions[i].z << ", m" << instances[i] << "));" << std::endl;
	}
	file << "// Globals:" << std::endl;
	file << "Indigo::Current_World.View.Place(" << Indigo::Current_World.View.X << ", " << Indigo::Current_World.View.Y << ", " << Indigo::Current_World.View.Z << ");" << std::endl;
	return true;
}

void Add_Object_To_World_Save(glm::vec3& position, const std::string& mesh, const std::string& texture)
{
	unsigned short index = 0;
	Mesh_Texture together = { mesh, texture };
	std::map<Mesh_Texture, unsigned short>::iterator location = model_to_index.find(together);
	if (location == model_to_index.end())
	{
		models.push_back(together);
		unsigned short index = models.size() - 1;
		instances.push_back(index);
		model_to_index[together] = index;
	}
	else
	{
		instances.push_back(location->second);
	}
	positions.push_back(position);
}

void Fade_Text(float time, Object& self)
{
	self.Data.Color.a -= 0.0002 * time;
	if (self.Data.Color.a < 0)
	{
		Indigo::Current_World.Remove_2D_Object(self.ID);
	}
}

void GUI(float time)
{
	static float total_speed = 0.005;
	static bool lighting_enabled = false;
	if (Position_Marker == -1)
	{
		Position_Marker = Indigo::Current_World.Add_2D_Object(Object(0, 0, 0, Mesh::Text("")));
	}
	if (!Typing)
	{
		if (Indigo::Keys['2'])
		{
			total_speed += 0.001 * total_speed * time;
		}
		if (Indigo::Keys['1'])
		{
			total_speed -= 0.001 * total_speed * time;
		}
		const float speed = total_speed * time;
		if (Indigo::Keys['w'])
		{
			To_Move->Move(speed);
		}
		if (Indigo::Keys['s'])
		{
			To_Move->Move(-speed);
		}
		if (Indigo::Keys['a'])
		{
			To_Move->Move(0, -speed);
		}
		if (Indigo::Keys['d'])
		{
			To_Move->Move(0, speed);
		}
		if (Indigo::Keys['e'])
		{
			To_Move->Move(0, 0, speed);
		}
		if (Indigo::Keys['q'])
		{
			To_Move->Move(0, 0, -speed);
		}
		Indigo::Current_World.Remove_2D_Object(Position_Marker);
		std::string display = std::to_string(To_Move->X) + "  " + std::to_string(To_Move->Y) + "  " + std::to_string(To_Move->Z);
		Position_Marker = Indigo::Current_World.Add_2D_Object(Object(-1.1, -0.9, 0, Mesh::Text(display.c_str(), 0.03)));
	}
	Indigo::Current_World.View.Place(Motion.X, Motion.Y, Motion.Z);
	Indigo::Current_World.View.Eye = Motion.Facing;
}

void Mouse_Interact(int button, int state, float x, float y)
{
	if (state == GLFW_PRESS)
	{
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			To_Move->Data.Color = glm::vec4(1.0, 1.0, 1.0, 1.0);
			int collide = Indigo::Current_World.Collide(Indigo::Mouse_Position);
			if (collide != -1)
			{
				Object * clicked = &Indigo::Current_World.Get_Object(collide);
				if (clicked != To_Move)
				{
					To_Move = clicked;
					To_Move->Data.Color = glm::vec4(300.0, 10.0, 10.0, 1.0);
				}
				else
				{
					To_Move = &Motion;
				}
			}
			else
			{
				To_Move = &Motion;
			}
		}
	}
}

void Mouse_Look(int x, int y)
{
	if (Indigo::Left_Mouse)
	{
		Motion.Facing.Add_Direction(0.0, x*0.2, y*-0.2);
		if (Indigo::Current_World.View.Eye.Get_Y_Angle() > 90 && Indigo::Current_World.View.Eye.Get_Y_Angle() < 270)
		{
			Indigo::Current_World.View.Up.Set_Y(-1.0);
		}
		else
		{
			Indigo::Current_World.View.Up.Set_Y(1.0);
		}
	}
}

bool Text_Edit(int key, std::string& output)
{
	if (key == GLFW_KEY_BACKSPACE)
	{
		if (output.length())
		{
			output.pop_back();
			return true;
		}
	}
	else if (key != GLFW_KEY_LEFT_SHIFT && key != GLFW_KEY_RIGHT_SHIFT && key != GLFW_KEY_LEFT_CONTROL && key != GLFW_KEY_RIGHT_CONTROL && key != GLFW_KEY_LEFT_ALT && key != GLFW_KEY_RIGHT_ALT)
	{
		if (Indigo::Shift)
		{
			key = Indigo::Get_Shifted_Character(key);
		}
		output += key;
		return true;
	}
	return false;
}

void Print(int& handle, const float x, const float y, const std::string toprint)
{
	Indigo::Current_World.Remove_2D_Object(handle);
	handle = Indigo::Current_World.Add_2D_Object(Object(x, y, 0, Mesh::Text(toprint.c_str(), 0.035)));
}

void Key_Pressed(int key)
{

	// Each menu that can be triggered by a key must have a handle
	static int space_menu = -1;
	static int save_menu = -1;
	static int open_menu = -1;
	static int position_menu = -1;
	static int facing_menu = -1;

	// Some menus have state variables that must be preserved
	static int pos_part = 0;
	static bool texture_yet = false;
	static bool compile;

	// Menus spawn near the mouse. Store where.
	static float menu_x = 0;
	static float menu_y = 0;

	// Each menu gathers and acts on data
	static std::string save_location = "";
	static std::string position = "";
	static std::string mesh = "";
	static std::string texture = "";
	static std::string facing = "";

	// Space Menu adds Models to the World
	if (space_menu != -1)
	{
		bool changed = false;
		if (key == GLFW_KEY_ENTER)
		{
			if (texture_yet)
			{
				Mesh add = Mesh(mesh.c_str(), texture.length() ? texture.c_str() : nullptr);
				float up = 0.035;
				if (add.Size && !(add.Texture_ID == 0 && texture.length()))
				{
					glm::vec3 position = glm::vec3(Indigo::Current_World.View.X, Indigo::Current_World.View.Y, Indigo::Current_World.View.Z);
					Add_Object_To_World_Save(position, mesh, texture);
					Indigo::Current_World.Add_Object(Object(position.x, position.y, position.z, add));
					Indigo::Current_World.Remove_2D_Object(space_menu);
					space_menu = -1;
					Typing = false;
				}
				else
				{
					if (add.Texture_ID == 0 && texture.length())
					{
						Indigo::Current_World.Add_2D_Object(Object(menu_x, menu_y + up, 0, Mesh::Text("Couldn't find BMP Texture.", 0.035), Fade_Text));
						up += 0.035;
					}
					if (!add.Size)
					{
						Indigo::Current_World.Add_2D_Object(Object(menu_x, menu_y + up, 0, Mesh::Text("Couldn't find OBJ.", 0.035), Fade_Text));
						texture_yet = false;
						changed = true;
					}
				}
			}
			else
			{
				if (!mesh.length())
				{
					Indigo::Current_World.Remove_2D_Object(space_menu);
					space_menu = -1;
					Typing = false;
				}
				else
				{
					texture_yet = true;
					if (texture == "")
					{
						texture = mesh.substr(0, mesh.length() - 4) + ".bmp";
					}
					changed = true;
				}
			}
		}
		else if (key == GLFW_KEY_ESCAPE)
		{
			if ((texture_yet ? texture : mesh).length() != 0)
			{
				if (texture_yet)
				{
					texture = "";
					Print(space_menu, menu_x, menu_y, "Texture: \n");
				}
				else
				{
					mesh = "";
					Print(space_menu, menu_x, menu_y, "Mesh: \n");
				}
			}
			else
			{
				if (texture_yet)
				{
					texture_yet = false;
					changed = true;
				}
				else
				{
					Indigo::Current_World.Remove_2D_Object(space_menu);
					space_menu = -1;
					Typing = false;
				}
			}
		}
		else
		{
			if (texture_yet)
			{
				changed = Text_Edit(key, texture);
			}
			else
			{
				changed = Text_Edit(key, mesh);
			}
		}
		if (changed)
		{
			if (!texture_yet)
			{
				texture = "";
			}
			Print(space_menu, menu_x, menu_y, (texture_yet ? ("Texture:\n" + texture) : ("Mesh:\n" + mesh)) + " ");
		}
	}

	// Save Menu saves the current World
	else if (save_menu != -1)
	{
		if (key == GLFW_KEY_ENTER)
		{
			bool success;
			if (compile)
			{
				success = Save_For_Compile(save_location.c_str());
			}
			else
			{
				success = Save(save_location.c_str());
			}
			if (Save(save_location.c_str()))
			{
				Indigo::Current_World.Remove_2D_Object(save_menu);
				save_menu = -1;
				Typing = false;
			}
			else
			{
				Indigo::Current_World.Add_Object(Object(menu_x, menu_y + 0.035, 0, Mesh::Text("Unable to save file. Check for write permissions.", 0.035), Fade_Text));
			}
		}
		else if (key == GLFW_KEY_ESCAPE)
		{
			if (save_location.length() != 0)
			{
				save_location = "";
				Print(save_menu, menu_x, menu_y, "Save Here:\n ");
			}
			else
			{
				Indigo::Current_World.Remove_2D_Object(save_menu);
				save_menu = -1;
				Typing = false;
			}
		}
		else if (Text_Edit(key, save_location))
		{
			Print(save_menu, menu_x, menu_y, "Save Here:\n" + save_location + " ");
		}
	}
	else if (open_menu != -1)
	{
		if (key == GLFW_KEY_ENTER)
		{
			if (Load(save_location.c_str()))
			{
				Indigo::Current_World.Remove_2D_Object(open_menu);
				open_menu = -1;
				Typing = false;
			}
			else
			{
				Indigo::Current_World.Add_Object(Object(menu_x, menu_y + 0.035, 0, Mesh::Text("Unable to open file.", 0.035), Fade_Text));
			}
		}
		else if (key == GLFW_KEY_ESCAPE)
		{
			if (save_location.length() != 0)
			{
				save_location = "";
				Print(open_menu, menu_x, menu_y, "Open From:\n ");
			}
			else
			{
				Indigo::Current_World.Remove_2D_Object(open_menu);
				open_menu = -1;
				Typing = false;
				save_location = "";
			}
		}
		else if (Text_Edit(key, save_location))
		{
			Print(open_menu, menu_x, menu_y, "Open From:\n" + save_location + " ");
		}
	}

	// Position Menu changes the camera or the active object's position
	else if (position_menu != -1)
	{
		bool changed = true;
		if (key == GLFW_KEY_ENTER)
		{
			if (position.length() == 0)
			{
				position = "0";
			}
			if (pos_part == 0)
			{
				To_Move->X = Indigo::Fast_Float(position.c_str());
				position = std::to_string(To_Move->Y);
			}
			if (pos_part == 1)
			{
				To_Move->Y = Indigo::Fast_Float(position.c_str());
				position = std::to_string(To_Move->Z);
			}
			if (pos_part == 2)
			{
				To_Move->Z = Indigo::Fast_Float(position.c_str());
				Indigo::Current_World.Remove_2D_Object(position_menu);
				position_menu = -1;
				Typing = false;
				changed = false;
			}
			pos_part++;
		}
		else if (key == GLFW_KEY_ESCAPE)
		{
			if (position.length() != 0)
			{
				position = "";
			}
			else if (pos_part > 0)
			{
				pos_part--;
			}
			else
			{
				Indigo::Current_World.Remove_2D_Object(position_menu);
				position_menu = -1;
				Typing = false;
				position = "";
				changed = false;
			}
		}
		else if (Text_Edit(key, position))
		{
			//
		}
		else
		{
			changed = false;
		}
		if (changed)
		{
			std::string which;
			if (pos_part == 0)
				which = "X:\n";
			if (pos_part == 1)
				which = "Y:\n";
			if (pos_part == 2)
				which = "Z:\n";
			Print(position_menu, menu_x, menu_y, which + position + " ");
		}
	}

	// Facing Menu changes the camera or the active object's direction
	else if (facing_menu != -1)
	{
		bool changed = true;
		if (key == GLFW_KEY_ENTER)
		{
			if (facing.length() == 0)
			{
				facing = "0";
			}
			if (pos_part == 0)
			{
				To_Move->Facing.Set_X_Angle(Indigo::Fast_Float(facing.c_str()));
				facing = std::to_string(To_Move->Facing.Get_Y_Angle());
			}
			if (pos_part == 1)
			{
				To_Move->Facing.Set_Y_Angle(Indigo::Fast_Float(facing.c_str()));
				Indigo::Current_World.Remove_2D_Object(facing_menu);
				facing_menu = -1;
				Typing = false;
				changed = false;
			}
			pos_part++;
		}
		else if (key == GLFW_KEY_ESCAPE)
		{
			if (facing.length() != 0)
			{
				facing = "";
			}
			else if (pos_part > 0)
			{
				pos_part--;
			}
			else
			{
				Indigo::Current_World.Remove_2D_Object(facing_menu);
				facing_menu = -1;
				Typing = false;
				facing = "";
				changed = false;
			}
		}
		else if (Text_Edit(key, facing))
		{
			//
		}
		else
		{
			changed = false;
		}
		if (changed)
		{
			std::string which;
			if (pos_part == 0)
				which = "X Angle:\n";
			if (pos_part == 1)
				which = "Y Angle:\n";
			Print(facing_menu, menu_x, menu_y, which + facing + " ");
		}
	}

	// These keys add menus
	else
	{
		if (key == ' ' || (Indigo::Control && key == 'a')) // Space
		{
			menu_x = Indigo::Mouse_Position.x + 0.07;
			menu_y = Indigo::Mouse_Position.y - 0.07;
			texture_yet = false;
			std::string display = "Mesh:\n" + mesh + " ";
			space_menu = Indigo::Current_World.Add_2D_Object(Object(menu_x, menu_y, 0, Mesh::Text(display.c_str(), 0.035)));
			Typing = true;
		}
		if (key == GLFW_KEY_TAB) // Position
		{
			menu_x = Indigo::Mouse_Position.x + 0.07;
			menu_y = Indigo::Mouse_Position.y - 0.07;
			pos_part = 0;
			position = std::to_string(To_Move->X);
			std::string display = "X:\n" + position + " ";
			position_menu = Indigo::Current_World.Add_2D_Object(Object(menu_x, menu_y, 0, Mesh::Text(display.c_str(), 0.035)));
			Typing = true;
		}
		if (key == 'f') // Facing
		{
			menu_x = Indigo::Mouse_Position.x + 0.07;
			menu_y = Indigo::Mouse_Position.y - 0.07;
			pos_part = 0;
			facing = std::to_string(To_Move->Facing.Get_X_Angle());
			std::string display = "X Angle:\n" + facing + " ";
			facing_menu = Indigo::Current_World.Add_2D_Object(Object(menu_x, menu_y, 0, Mesh::Text(display.c_str(), 0.035)));
			Typing = true;
		}
		if (key == GLFW_KEY_BACKSPACE || key == GLFW_KEY_DELETE || key == 'x')
		{
			if (To_Move != &Motion)
			{
				Indigo::Current_World.Remove_Object(To_Move->ID);
				To_Move = &Motion;
			}
		}
		if (Indigo::Control)
		{
			if (key == 's') // Save
			{
				menu_x = Indigo::Mouse_Position.x + 0.07;
				menu_y = Indigo::Mouse_Position.y - 0.07;
				std::string display = "Save Here:\n" + save_location + " ";
				save_menu = Indigo::Current_World.Add_2D_Object(Object(menu_x, menu_y, 0, Mesh::Text(display.c_str(), 0.035)));
				Typing = true;
				compile = Indigo::Shift;
			}
			if (key == 'o') // Open (Save)
			{
				menu_x = Indigo::Mouse_Position.x + 0.07;
				menu_y = Indigo::Mouse_Position.y - 0.07;
				std::string display = "Open From:\n" + save_location + " ";
				open_menu = Indigo::Current_World.Add_2D_Object(Object(menu_x, menu_y, 0, Mesh::Text(display.c_str(), 0.035)));
				Typing = true;
			}
			if (key == 'n') // New
			{
				model_to_index = std::map<Mesh_Texture, unsigned short>();
				models = std::vector<Mesh_Texture>();
				instances = std::vector<unsigned short>();
				positions = std::vector<glm::vec3>();
				Indigo::Current_World = restore;
			}
		}
	}
}

int main()
{
	Indigo::Initialize("Indigo Engine Level Designer", Indigo::Sky_Color, 1280, 720, 24, false);
	Indigo::Update_Function = GUI;
	Indigo::Current_World.Shader("CodeIndigo/Indigo/Shaders/Default.vs", "CodeIndigo/Indigo/Shaders/Default.fs");
	Indigo::Current_World.Light_Setup.Set_Ambient(0.075);
	Indigo::Current_World.Light_Setup.Set_Light(0, -1, 0, false);
	Indigo::Mouse_Button_Function = Mouse_Interact;
	Indigo::Relative_Mouse_Moved_Function = Mouse_Look;
	Indigo::Key_Pressed_Function = Key_Pressed;
	restore = Indigo::Current_World;
	Indigo::Run();
}
