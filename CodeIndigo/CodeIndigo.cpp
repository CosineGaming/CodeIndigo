#include "Indigo/IndigoEngine.h"
#include <iostream>
#include <fstream>
#include <string>


std::vector<std::string> models;
std::vector<std::string> textures;
std::vector<glm::vec3> coordinates;

bool Typing;

void Fade_Text(float time, Object& self)
{
	self.Data.Color.a -= 0.0002 * time;
	if (self.Data.Color.a < 0)
	{
		Indigo::Current_World.Remove_2D_Object(self);
	}
}

void GUI(float time)
{
	static float total_speed = 0.005;
	static bool lighting_enabled = false;
	if (!Typing)
	{
		const float speed = total_speed * time;
		if (Indigo::Keys['w'])
		{
			Indigo::Current_World.View.Move(speed);
		}
		if (Indigo::Keys['s'])
		{
			Indigo::Current_World.View.Move(-speed);
		}
		if (Indigo::Keys['a'])
		{
			Indigo::Current_World.View.Move(0, -speed);
		}
		if (Indigo::Keys['d'])
		{
			Indigo::Current_World.View.Move(0, speed);
		}
		if (Indigo::Keys['e'])
		{
			Indigo::Current_World.View.Move(0, 0, speed);
		}
		if (Indigo::Keys['q'])
		{
			Indigo::Current_World.View.Move(0, 0, -speed);
		}
		if (Indigo::Keys['2'])
		{
			total_speed += 0.001 * total_speed * time;
		}
		if (Indigo::Keys['1'])
		{
			total_speed -= 0.001 * total_speed * time;
		}
	}
}

void Mouse_Interact(int x, int y)
{
	//
}

void Mouse_Look(int x, int y)
{
	if (Indigo::Left_Mouse)
	{
		Indigo::Current_World.View.Eye.Add_Direction(0.0, x*0.2, y*-0.2);
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

void Key_Pressed(int key)
{
	static int space_menu = -1;
	static int save_menu = -1;
	static bool texture_yet = false;
	static float menu_x = 0;
	static float menu_y = 0;
	static std::string save_location = "";
	static std::string model = "";
	static std::string texture = "";
	if (space_menu != -1) // Typing in the Model Adder
	{
		bool changed = false;
		if (key == GLFW_KEY_ENTER)
		{
			if (texture_yet)
			{
				Mesh add = Mesh(model.c_str(), texture.length() ? texture.c_str() : nullptr);
				float up = 0.035;
				if (add.Size && !(add.Texture_ID == 0 && texture.length()))
				{
					models.push_back(model);
					textures.push_back(texture);
					glm::vec3 position = glm::vec3(Indigo::Current_World.View.X, Indigo::Current_World.View.Y, Indigo::Current_World.View.Z);
					coordinates.push_back(position);
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
				if (!model.length())
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
						texture = model.substr(0, model.length() - 4) + ".bmp";
					}
					changed = true;
				}
			}
		}
		else if (key == GLFW_KEY_ESCAPE)
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
		if (!changed)
		{
			if (texture_yet)
			{
				changed = Text_Edit(key, texture);
			}
			else
			{
				changed = Text_Edit(key, model);
			}
		}
		if (changed)
		{
			Indigo::Current_World.Remove_2D_Object(space_menu);
			std::string display = (texture_yet ? ("Texture:\n" + texture) : ("Model:\n" + model)) + " ";
			space_menu = Indigo::Current_World.Add_2D_Object(Object(menu_x, menu_y, 0, Mesh::Text(display.c_str(), 0.035)));
		}
	}
	else
	{
		if (key == ' ')
		{
			menu_x = Indigo::Mouse_Position.x;
			menu_y = Indigo::Mouse_Position.y;
			texture_yet = false;
			std::string display = "Model:\n" + model + " ";
			space_menu = Indigo::Current_World.Add_2D_Object(Object(menu_x, menu_y, 0, Mesh::Text(display.c_str(), 0.035)));
			Typing = true;
			return;
		}
		if (key == GLFW_KEY_TAB)
		{
			if (save_menu == -1)
			{
				std::string display = "Save here:\n ";
				save_menu = Indigo::Current_World.Add_2D_Object(Object(menu_x, menu_y, 0, Mesh::Text(display.c_str(), 0.035)));
			}
			else
			{
				std::ofstream file(save_location, std::ios::out);
				for (int i = 0; i < models.size(); ++i)
				{
					if (textures[i][0] != '\0')
					{
						std::cout << "Indigo::Current_World.Add_Object(Object(" << coordinates[i].x << ", " << coordinates[i].y << ", " << coordinates[i].z << ", " << "Mesh(\"" << models[i] << "\", \"" << textures[i] << "\")));" << std::endl;
					}
					else
					{
						std::cout << "Indigo::Current_World.Add_Object(Object(" << coordinates[i].x << ", " << coordinates[i].y << ", " << coordinates[i].z << ", " << "Mesh(\"" << models[i] << "\")));" << std::endl;
					}
				}
			}
		}
	}
}

int main()
{
	Indigo::Initialize("Indigo Engine Level Designer", Indigo::Sky_Color, 1280, 720, 24, false);
	Indigo::Update_Function = GUI;
	Indigo::Current_World.Shader("Default.vs", "Default.fs");
	Indigo::Current_World.Light_Setup.Set_Ambient(0.075);
	Indigo::Current_World.Light_Setup.Set_Light(0, -1, 0, true);
	Indigo::Mouse_Moved_Function = Mouse_Interact;
	Indigo::Relative_Mouse_Moved_Function = Mouse_Look;
	Indigo::Key_Pressed_Function = Key_Pressed;
	//Indigo::Current_World.Add_Object(Object(0, 0, 0, Mesh("C:/Users/Judah/Documents/Frost/SpawnTunnel.obj", "C:/Users/Judah/Documents/Frost/SpawnTunnel.bmp")));
	Indigo::Run();
}
