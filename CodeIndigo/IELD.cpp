#include "IELD.h"


World restore;

Object Motion;
Object * To_Move = &Motion;

std::vector<Mesh> meshes;
std::vector<std::string> objs;
std::vector<std::string> textures;

int Position_Marker = -1;
bool Render_Marker = true;

bool Snap;

int Undo_Index = -1;
Object Undo_Object = Object();

std::string Typing = "";

int Current_Menu = -1;
int Cursor_Pos = 0;
float Cursor_Delay = 0;
bool Cursor_On = false;

void Fade_Text(float time, Object& self)
{
	self.Color.a -= 0.0002 * time;
	if (self.Color.a < 0)
	{
		Indigo::Current_World.Remove_2D_Object(self.ID);
	}
}

bool Text_Edit(int key, std::string& output)
{
	if (key == GLFW_KEY_ESCAPE)
	{
		if (output.length() != 0)
		{
			output = "";
			Cursor_Pos = 0;
			return true;
		}
	}
	else if (key == GLFW_KEY_BACKSPACE)
	{
		if (Cursor_Pos > 0 && Cursor_Pos <= output.length())
		{
			output.erase(output.begin() + (Cursor_Pos - 1));
			Cursor_Pos--;
			return true;
		}
	}
	else if (key == GLFW_KEY_DELETE)
	{
		if (Cursor_Pos < output.length())
		{
			output.erase(output.begin() + Cursor_Pos);
			return true;
		}
	}
	else if (key == GLFW_KEY_LEFT)
	{
		Cursor_Pos--;
		if (Cursor_Pos < 0)
		{
			Cursor_Pos = 0;
		}
		return true;
	}
	else if (key == GLFW_KEY_RIGHT)
	{
		Cursor_Pos++;
		if (Cursor_Pos > output.length())
		{
			Cursor_Pos = output.length();
		}
		return true;
	}
	else if (key != GLFW_KEY_LEFT_SHIFT && key != GLFW_KEY_RIGHT_SHIFT && key != GLFW_KEY_LEFT_CONTROL && key != GLFW_KEY_RIGHT_CONTROL && key != GLFW_KEY_LEFT_ALT && key != GLFW_KEY_RIGHT_ALT)
	{
		if (Indigo::Shift)
		{
			key = Indigo::Get_Shifted_Character(key);
		}
		output.insert(Cursor_Pos, 1, key);
		Cursor_Pos++;
		return true;
	}
	return false;
}

void Print(const int handle, const std::string toprint)
{
	Object& affect = Indigo::Current_World.Get_2D_Object(handle);
	affect.Data = Mesh::Text(toprint.c_str(), 0.05, "Textures/Font.png", glm::vec4(0, 0, 0, 1));
	if (affect.X + affect.Data.Hitbox[0].x < -1 * Indigo::Aspect_Ratio)
	{
		affect.X = -1 * Indigo::Aspect_Ratio - affect.Data.Hitbox[0].x;
	}
	if (affect.X + affect.Data.Hitbox[1].x > Indigo::Aspect_Ratio)
	{
		affect.X = Indigo::Aspect_Ratio - affect.Data.Hitbox[1].x;
	}
	if (affect.Y + affect.Data.Hitbox[0].y < -1)
	{
		affect.Y = -1 - affect.Data.Hitbox[0].y;
	}
}

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
			int next;
			float x = Indigo::Fast_Float(line.c_str(), &next);
			float y = Indigo::Fast_Float(line.c_str(), &next, next + 1);
			float z = Indigo::Fast_Float(line.c_str(), nullptr, next + 1);
			Motion.Place(x, y, z);
		}
		if (state == 1)
		{
			int next;
			unsigned short index = Indigo::Fast_Float(line.c_str(), &next);
			float x = Indigo::Fast_Float(line.c_str(), &next, next+1);
			float y = Indigo::Fast_Float(line.c_str(), &next, next+1);
			float z = Indigo::Fast_Float(line.c_str(), &next, next+1);
			float ax = Indigo::Fast_Float(line.c_str(), &next, next + 1);
			float ay = Indigo::Fast_Float(line.c_str(), nullptr, next + 1);
			int sep = line.find("|", next + 1);
			std::string mesh = line.substr(next + 1, sep);
			std::string texture = line.substr(sep + 1);
			objs.push_back(mesh);
			textures.push_back(texture);
			meshes.push_back(Mesh(mesh.c_str(), texture.c_str()));
			Indigo::Current_World.Add_Object(Object(x, y, z, meshes[index], nullptr, 0, Direction(1, ax, ay)));
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
	file << "// World file for the Indigo Engine" << std::endl << "// Auto-generated by the Indigo Engine Level Designer" << std::endl;
	file << "> Globals:" << std::endl;
	file << Indigo::Current_World.View.X << "|" << Indigo::Current_World.View.Y << "|" << Indigo::Current_World.View.Z << std::endl;
	file << "> Objects:" << std::endl;
	for (int i = 0; i < Indigo::Current_World.Number_Of_Objects(); ++i)
	{
		Object& object = Indigo::Current_World.Get_Object(i);
		if (object.Data.Size != 0)
		{
			file << object.X << "|" << object.Y << "|" << object.Z << "|" << object.Facing.Get_X_Angle() << "|" << object.Facing.Get_Y_Angle() << "|" << objs[i] << "|" << textures[i] << std::endl;
		}
	}
	return true;
}

bool Save_For_Compile(const char * filename)
{
	std::ofstream file(filename);
	if (!file)
	{
		return false;
	}
	file << "// World file for the Indigo Engine" << std::endl << "// Auto-generated by the Indigo Engine Level Designer" << std::endl;
	file << "// Actual C++ Indigo Engine code for #including in loading routine" << std::endl;
	file << "// Globals:" << std::endl;
	file << "Indigo::Current_World.View.Place(" << Indigo::Current_World.View.X << ", " << Indigo::Current_World.View.Y << ", " << Indigo::Current_World.View.Z << ");" << std::endl;
	file << "// Objects:" << std::endl;
	for (int i = 0; i < Indigo::Current_World.Number_Of_Objects(); ++i)
	{
		Object& object = Indigo::Current_World.Get_Object(i);
		if (object.Data.Size != 0)
		{
			file << "Indigo::Current_World.Add_Object(Object(" << object.X << ", " << object.Y << ", " << object.Z
				<< ", Mesh(" << objs[i] << ", " << textures[i] << "), nullptr, Direction(1, "
				<< object.Facing.Get_X_Angle() << ", " << object.Facing.Get_Y_Angle() << ")));" << std::endl;
		}
	}
	return true;
}

bool Add_And_Save(glm::vec3& position, const std::string& mesh, const std::string& texture)
{
	Mesh add = Mesh(mesh.c_str(), texture.length() ? texture.c_str() : nullptr);
	if (add.Size == 0)
	{
		return false;
	}
	meshes.push_back(add);
	objs.push_back(mesh);
	textures.push_back(texture);
	Indigo::Current_World.Add_Object(Object(position.x, position.y, position.z, add));
	return true;
}

void Mouse_Interact(int button, int state, float x, float y)
{
	if (state == GLFW_RELEASE)
	{
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			To_Move->Color /= 1.8;
			int collide = Indigo::Current_World.Collide(Indigo::Mouse_Position);
			if (collide != -1)
			{
				Object * clicked = &Indigo::Current_World.Get_Object(collide);
				if (clicked != To_Move)
				{
					To_Move = clicked;
					To_Move->Color *= 1.8;
					Undo_Index = clicked->ID;
					Undo_Object = *clicked;
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
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (Current_Menu != -1)
			{
				Object& menu = Indigo::Current_World.Get_2D_Object(Current_Menu);
				Cursor_Pos = int((x - menu.X) / 0.05 + 0.5);
				if (Cursor_Pos < 0)
				{
					Cursor_Pos = 0;
				}
				if (Cursor_Pos > Typing.length())
				{
					Cursor_Pos = Typing.length();
				}
				Cursor_On = true;
				Cursor_Delay = 0;
			}
		}
	}
}

void Mouse_Look(int x, int y)
{
	if (Indigo::Left_Mouse)
	{
		Motion.Facing.Add_Direction(0.0, x*0.2, y*-0.2);
		if (Motion.Facing.Get_Y_Angle() > 90 && Motion.Facing.Get_Y_Angle() < 270)
		{
			Indigo::Current_World.View.Up.Set_Y(-1.0);
		}
		else
		{
			Indigo::Current_World.View.Up.Set_Y(1.0);
		}
	}
}

void GUI(float time)
{
	static float total_speed = 0.005;
	static bool lighting_enabled = false;
	static int cursor_object = -1;
	if (cursor_object == -1)
	{
		cursor_object = Indigo::Current_World.Add_2D_Object(Object(0, 0, 0, Mesh::Text("|", 0.05)));
	}
	if (Current_Menu == -1)
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
		if (Position_Marker == -1)
		{
			Position_Marker = Indigo::Current_World.Add_2D_Object(Object());
		}
		Object& marker = Indigo::Current_World.Get_2D_Object(Position_Marker);
		if (Indigo::Elapsed() > 10000 && Render_Marker)
		{
			std::string display = "Indigo Engine Level Designer\n"
				"Objects  " + std::to_string(Indigo::Current_World.Number_Of_Objects()) + "\n"
				+ "FPS  " + std::to_string(Indigo::Actual_FPS) + "  MPF  " + std::to_string(int(time)) + "\n"
				+ "Camera Speed  " + std::to_string(int(total_speed * 1000)) + "  M/S" "\n"
				+ "Position  " + std::to_string(To_Move->X) + "  " + std::to_string(To_Move->Y) + "  " + std::to_string(To_Move->Z) + "\n"
				+ "Press H for help";
			marker.Data = Mesh::Text(display.c_str(), 0.05, "Textures/Font.png", glm::vec4(0, 0, 0, 1));
			marker.X = -1 * Indigo::Aspect_Ratio + 0.1;
			marker.Y = -0.9 - marker.Data.Hitbox[0].y;
		}
		else
		{
			marker.Data = Mesh();
		}
		Indigo::Current_World.Get_2D_Object(cursor_object).X = -100;
	}
	else
	{
		static float backspace = 0;
		static bool already = false;
		if (Indigo::Keys[GLFW_KEY_BACKSPACE])
		{
			backspace += time;
			if ((already && backspace > 50) || (!already && backspace > 500))
			{
				backspace = 0;
				already = true;
				Key_Pressed(GLFW_KEY_BACKSPACE);
			}
		}
		else
		{
			backspace = 0;
			already = false;
		}

		if (cursor_object < Current_Menu) // The cursor must be always on top
		{
			Indigo::Current_World.Remove_2D_Object(cursor_object);
			cursor_object = Indigo::Current_World.Add_2D_Object(Object(0, 0, 0, Mesh::Text("|", 0.05, "Textures/Font.png", glm::vec4(1, 1, 1, 1))));
		}
		Cursor_Delay += time;
		Object& marker = Indigo::Current_World.Get_2D_Object(cursor_object);
		if (Cursor_Delay > 750)
		{
			Cursor_Delay = 0;
			Cursor_On = !Cursor_On;
		}
		if (!Cursor_On)
		{
			marker.X = -100;
		}
		else
		{
			Object& menu = Indigo::Current_World.Get_2D_Object(Current_Menu);
			marker.X = menu.X + Cursor_Pos * 0.05 - 0.0075;
			marker.Y = menu.Y + menu.Data.Hitbox[0].y - ((Typing.length() == 0) ? 0.05 : 0);
		}
	}
	Indigo::Current_World.View.Place(Motion.X, Motion.Y, Motion.Z);
	Indigo::Current_World.View.Eye = Motion.Facing;
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

	// User input may be remembered for easier input in the future
	static std::string save_location = "";
	static std::string mesh = "";
	static std::string texture = "";

	Cursor_On = true;
	Cursor_Delay = 0;

	// Space Menu adds Models to the World
	if (space_menu != -1)
	{
		bool changed = false;
		if (key == GLFW_KEY_ESCAPE && Typing.length() == 0)
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
				Current_Menu = -1;
			}
		}
		if ((key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER))
		{
			if (texture_yet)
			{
				Print(space_menu, "Loading...");
				Indigo::Render();
				texture = Typing;
				glm::vec3 position = glm::vec3(Indigo::Current_World.View.X, Indigo::Current_World.View.Y, Indigo::Current_World.View.Z);
				if (Add_And_Save(position, mesh.c_str(), texture.length() ? texture.c_str() : ""))
				{
					Undo_Index = Indigo::Current_World.Number_Of_Objects() - 1;
					Undo_Object = Object();
					Indigo::Current_World.Remove_2D_Object(space_menu);
					space_menu = -1;
					Current_Menu = -1;
				}
				else
				{
					Indigo::Current_World.Add_2D_Object(Object(menu_x, menu_y + 0.05, 0, Mesh::Text("Couldn't find / load OBJ.", 0.05, "Textures/Font.png", glm::vec4(0,0,0,1)), Fade_Text));
					texture_yet = false;
					changed = true;
				}
			}
			else
			{
				mesh = Typing;
				if (!mesh.length())
				{
					Indigo::Current_World.Remove_2D_Object(space_menu);
					space_menu = -1;
					Current_Menu = -1;
					mesh = "";
				}
				else
				{
					texture_yet = true;
					if (texture == "")
					{
						texture = mesh.substr(0, mesh.length() - 4) + ".png";
					}
					Typing = texture;
					changed = true;
				}
			}
		}
		else
		{
			changed = Text_Edit(key, Typing);
		}
		if (changed)
		{
			if (!texture_yet)
			{
				texture = "";
			}
			Print(space_menu, (texture_yet ? ("Texture:\n" + Typing) : ("Mesh:\n" + Typing)));
		}
	}

	// Save Menu saves the current World
	else if (save_menu != -1)
	{
		if (key == GLFW_KEY_ESCAPE && Typing.length() == 0)
		{
			Indigo::Current_World.Remove_2D_Object(save_menu);
			save_menu = -1;
			Current_Menu = -1;
		}
		if ((key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER))
		{
			Print(save_menu, "Loading...");
			Indigo::Render();
			bool success;
			save_location = Typing;
			if (compile)
			{
				success = Save_For_Compile(Typing.c_str());
			}
			else
			{
				success = Save(Typing.c_str());
			}
			if (success)
			{
				Indigo::Current_World.Remove_2D_Object(save_menu);
				save_menu = -1;
				Current_Menu = -1;
			}
			else
			{
				Indigo::Current_World.Add_2D_Object(Object(menu_x, menu_y + 0.05, 0, Mesh::Text("Unable to save file. Check for write permissions.", 0.05, "Texture/Font.png", glm::vec4(1, 1, 1, 1)), Fade_Text));
				Print(save_menu, "Save Here:\n" + Typing);
			}
		}
		else if (Text_Edit(key, Typing))
		{
			Print(save_menu, "Save Here:\n" + Typing);
		}
	}
	else if (open_menu != -1)
	{
		if (key == GLFW_KEY_ESCAPE && Typing.length() == 0)
		{
			Indigo::Current_World.Remove_2D_Object(open_menu);
			open_menu = -1;
			Current_Menu = -1;
		}
		if ((key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER))
		{
			Print(open_menu, "Loading...");
			Indigo::Render();
			save_location = Typing;
			if (Load(Typing.c_str()))
			{
				Indigo::Current_World.Remove_2D_Object(open_menu);
				open_menu = -1;
				Current_Menu = -1;
			}
			else
			{
				Indigo::Current_World.Add_2D_Object(Object(menu_x, menu_y + 0.05, 0, Mesh::Text("Unable to open file.", 0.05, "Textures/Font.png", glm::vec4(0, 0, 0, 1)), Fade_Text));
				Print(open_menu, "Open From:\n" + Typing);
			}
		}
		else if (Text_Edit(key, Typing))
		{
			Print(open_menu, "Open From:\n" + Typing);
		}
	}

	// Position Menu changes the camera or the active object's position
	else if (position_menu != -1)
	{
		bool changed = true;
		if (key == GLFW_KEY_ESCAPE && Typing.length() == 0)
		{
			if (pos_part > 0)
			{
				pos_part--;
			}
			else
			{
				Indigo::Current_World.Remove_2D_Object(position_menu);
				position_menu = -1;
				Current_Menu = -1;
				changed = false;
			}
		}
		if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER)
		{
			if (Typing.length() == 0)
			{
				Typing = "0";
			}
			if (pos_part == 0)
			{
				To_Move->X = Indigo::Fast_Float(Typing.c_str());
				Typing = std::to_string(To_Move->Y);
			}
			if (pos_part == 1)
			{
				To_Move->Y = Indigo::Fast_Float(Typing.c_str());
				Typing = std::to_string(To_Move->Z);
			}
			if (pos_part == 2)
			{
				To_Move->Z = Indigo::Fast_Float(Typing.c_str());
				Indigo::Current_World.Remove_2D_Object(position_menu);
				position_menu = -1;
				Current_Menu = -1;
				changed = false;
			}
			pos_part++;
		}
		else if (Text_Edit(key, Typing))
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
			Print(position_menu, which + Typing);
		}
	}

	// Facing Menu changes the camera or the active object's direction
	else if (facing_menu != -1)
	{
		bool changed = true;
		if (key == GLFW_KEY_ESCAPE && Typing.length() == 0)
		{
			if (pos_part > 0)
			{
				pos_part--;
			}
			else
			{
				Indigo::Current_World.Remove_2D_Object(facing_menu);
				facing_menu = -1;
				Current_Menu = -1;
				Typing = "";
				changed = false;
			}
		}
		if ((key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER))
		{
			if (Typing.length() == 0)
			{
				Typing = "0";
			}
			if (pos_part == 0)
			{
				To_Move->Facing.Set_X_Angle(Indigo::Fast_Float(Typing.c_str()));
				Typing = std::to_string(To_Move->Facing.Get_Y_Angle());
			}
			if (pos_part == 1)
			{
				To_Move->Facing.Set_Y_Angle(Indigo::Fast_Float(Typing.c_str()));
				Indigo::Current_World.Remove_2D_Object(facing_menu);
				facing_menu = -1;
				Current_Menu = -1;
				changed = false;
			}
			pos_part++;
		}
		else if (Text_Edit(key, Typing))
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
			Print(facing_menu, which + Typing);
		}
	}

	// These keys add menus
	else
	{
		if (key == ' ' || (Indigo::Control && key == 'a')) // Space (or Ctrl+A)
		{
			menu_x = Indigo::Mouse_Position.x + 0.07;
			menu_y = Indigo::Mouse_Position.y - 0.07;
			texture_yet = false;
			Typing = mesh;
			std::string display = "Mesh:\n" + Typing;
			space_menu = Indigo::Current_World.Add_2D_Object(Object(menu_x, menu_y));
			Current_Menu = space_menu;
			Print(space_menu, display);
		}
		if (key == GLFW_KEY_TAB) // Position
		{
			Undo_Index = To_Move->ID;
			Undo_Object = *To_Move;
			menu_x = Indigo::Mouse_Position.x + 0.07;
			menu_y = Indigo::Mouse_Position.y - 0.07;
			pos_part = 0;
			Typing = std::to_string(To_Move->X);
			std::string display = "X:\n" + Typing;
			position_menu = Indigo::Current_World.Add_2D_Object(Object(menu_x, menu_y));
			Current_Menu = position_menu;
			Print(position_menu, display);
		}
		if (key == 'f') // Facing
		{
			Undo_Index = To_Move->ID;
			Undo_Object = *To_Move;
			menu_x = Indigo::Mouse_Position.x + 0.07;
			menu_y = Indigo::Mouse_Position.y - 0.07;
			pos_part = 0;
			Typing = std::to_string(To_Move->Facing.Get_X_Angle());
			std::string display = "X Angle:\n" + Typing;
			facing_menu = Indigo::Current_World.Add_2D_Object(Object(menu_x, menu_y));
			Current_Menu = facing_menu;
			Print(facing_menu, display);
		}
		if (key == GLFW_KEY_BACKSPACE || key == GLFW_KEY_DELETE || key == 'x') // Delete
		{
			if (To_Move != &Motion)
			{
				Undo_Index = To_Move->ID;
				Undo_Object = *To_Move;
				Indigo::Current_World.Remove_Object(To_Move->ID);
				To_Move = &Motion;
			}
		}
		if (key == 'h') // Help
		{
			// Because text file won't raise permissions and is very unlikely to be exploited, System seems a viable option
#ifdef _WIN32
			system("\"IELD ReadMe.txt\""); // Windows
#endif
#ifdef __APPLE__
			system("open -t \"IELD ReadMe.txt\""); // Mac
#endif
#ifdef __LINUX__
			system("xdg-open \"IELD ReadMe.txt\""); // Linux
#endif
		}
		if (key == 'p') 
		{
			Render_Marker = !Render_Marker;
		}
		if (Indigo::Control)
		{
			if (key == 's') // Save
			{
				menu_x = Indigo::Mouse_Position.x + 0.07;
				menu_y = Indigo::Mouse_Position.y - 0.07;
				Typing = save_location;
				std::string display = "Save Here:\n" + Typing;
				save_menu = Indigo::Current_World.Add_2D_Object(Object(menu_x, menu_y));
				Current_Menu = save_menu;
				Print(save_menu, display);
				compile = Indigo::Shift;
			}
			if (key == 'o') // Open (Save)
			{
				menu_x = Indigo::Mouse_Position.x + 0.07;
				menu_y = Indigo::Mouse_Position.y - 0.07;
				Typing = save_location;
				std::string display = "Open From:\n" + Typing;
				open_menu = Indigo::Current_World.Add_2D_Object(Object(menu_x, menu_y));
				Current_Menu = open_menu;
				Print(open_menu, display);
			}
			if (key == 'n') // New
			{
				meshes = std::vector<Mesh>();
				objs = std::vector<std::string>();
				textures = std::vector<std::string>();
				To_Move = &Motion;
				Undo_Index = -1;
				Position_Marker = -1;
				Indigo::Current_World = restore;
			}
			if (key == 'z') // Undo
			{
				if (Undo_Index != -1)
				{
					Indigo::Current_World.Get_Object(Undo_Index) = Undo_Object;
				}
			}
		}
		Cursor_Pos = Typing.length();
	}
}

int main(int argc, char ** argv)
{
	Indigo::Initialize("IELD", Indigo::Sky_Color, 1, 24, -240, -135, false);
	Indigo::Update_Function = GUI;
	Indigo::Current_World.Shader("Indigo/Shaders/Default.vs", "Indigo/Shaders/Default.fs");
	Indigo::Current_World.Light_Setup.Set_Ambient(0.075);
	//Indigo::Current_World.Light_Setup.Add_Sun(0, -1, 0);
	Indigo::Current_World.Light_Setup.Add_Bulb(0, 1.88, 0, 0.5);
	Indigo::Current_World.Light_Setup.Add_Lamp(-0.35662, 0.6792, 1.92358, glm::vec3(-1, -1, 0), 90, 0.4);
	Indigo::Mouse_Button_Function = Mouse_Interact;
	Indigo::Relative_Mouse_Moved_Function = Mouse_Look;
	Indigo::Key_Pressed_Function = Key_Pressed;
	restore = Indigo::Current_World;
	//Indigo::Construct_Splash();
	if (argc > 1)
	{
		Load(argv[1]);
	}
	Indigo::Run();
}
