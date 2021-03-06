#include "IELD.h"
#ifdef _WIN32
// For the file dialogue
#include "windows.h"
#endif


World Restore;

Object Motion;
Object * To_Move;

std::vector<std::string> Objs;
std::vector<std::string> Textures;
std::vector<std::string> Bump_Maps;

float Camera_Speed;

bool Physics; // Todo: Implement

int Position_Marker;
bool Render_Marker;

float Snap;

int Undo_Index;
Object Undo_Object;

int Copied;

std::string Typing;

int Current_Menu;
int Cursor_Pos;
float Cursor_Delay;
bool Cursor_On;

const float Splash_Time = 10;

unsigned int tex1;
unsigned int tex2;
unsigned int tex12;
unsigned int tex21;

void Fade_Text(float time, Object& self)
{
	self.Color.a -= 2 * time;
	if (self.Color.a < 0)
	{
		Indigo::Worlds[0].Remove_2D_Object(self.ID);
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
	Object& affect = Indigo::Worlds[0].Get_2D_Object(handle);
	affect.Data = Mesh::Text(toprint.c_str(), 0.05, "Textures/Font.png", glm::vec4(0, 0, 0, 1), glm::vec4(Indigo::Sky_Color, 0.9));
	if (affect.X + affect.Data.Hitbox[0].x < -1 * Indigo::Aspect_Ratio)
		affect.X = -1 * Indigo::Aspect_Ratio - affect.Data.Hitbox[0].x;
	if (affect.X + affect.Data.Hitbox[1].x > Indigo::Aspect_Ratio)
		affect.X = Indigo::Aspect_Ratio - affect.Data.Hitbox[1].x;
	if (affect.Y + affect.Data.Hitbox[0].y < -1)
		affect.Y = -1 - affect.Data.Hitbox[0].y;
}

bool Load(const char * filename, const bool render)
{
	/* File Format:
	// Comments usual style
	// First line is globals
	camx|camy|camz|camspeed|snap
	// Subsequent lines are each objects
	x|y|z|facex|facey|r|g|b|a|shine|model|texture|bumpmap
	x|y|...
	*/
	// Covers all feasibly implementable object properties
	std::ifstream file(filename);
	if (!file)
	{
		return false;
	}
	std::string line;
	bool first_line = true;
	while (std::getline(file, line))
	{

		if (line[0] == '/' && line[1] == '/')
		{
			continue;
		}
		
		int next;

		if (first_line)
		{
			float x = Indigo::Fast_Float(line.c_str(), &next);
			float y = Indigo::Fast_Float(line.c_str(), &next, next + 1);
			float z = Indigo::Fast_Float(line.c_str(), &next, next + 1);
			float ax = Indigo::Fast_Float(line.c_str(), &next, next + 1);
			float ay = Indigo::Fast_Float(line.c_str(), &next, next + 1);
			Camera_Speed = Indigo::Fast_Float(line.c_str(), &next, next + 1);
			Snap = Indigo::Fast_Float(line.c_str(), &next, next + 1);
			Motion.Place(x, y, z);
			Motion.Facing.Set_Direction(1, ax, ay);
			first_line = false;
		}

		else
		{
			float x = Indigo::Fast_Float(line.c_str(), &next);
			float y = Indigo::Fast_Float(line.c_str(), &next, next + 1);
			float z = Indigo::Fast_Float(line.c_str(), &next, next + 1);
			float ax = Indigo::Fast_Float(line.c_str(), &next, next + 1);
			float ay = Indigo::Fast_Float(line.c_str(), &next, next + 1);
			float r = Indigo::Fast_Float(line.c_str(), &next, next + 1);
			float g = Indigo::Fast_Float(line.c_str(), &next, next + 1);
			float b = Indigo::Fast_Float(line.c_str(), &next, next + 1);
			float a = Indigo::Fast_Float(line.c_str(), &next, next + 1);
			float shine = Indigo::Fast_Float(line.c_str(), &next, next + 1);
			int mesh_end = line.find("|", next + 1);
			int texture_end = line.find("|", mesh_end + 1);
			std::string mesh = line.substr(next + 1, mesh_end - next - 1);
			std::string texture = line.substr(mesh_end + 1, texture_end - mesh_end - 1);
			std::string bump_map = line.substr(texture_end + 1);
			Objs.push_back(mesh);
			Textures.push_back(texture);
			Bump_Maps.push_back(bump_map);
			Indigo::Worlds[0].Add_Object(Object(x, y, z, Mesh(mesh.c_str(), texture.c_str(), bump_map.c_str()), nullptr, shine, Direction(1, ax, ay), glm::vec4(r, g, b, a)));
		}

		if (render)
		{
			Indigo::Render();
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
	file << Indigo::Worlds[0].Views[0].X << "|" << Indigo::Worlds[0].Views[0].Y << "|" << Indigo::Worlds[0].Views[0].Z
		<< "|" << Indigo::Worlds[0].Views[0].Eye.Get_X_Angle() << "|" << Indigo::Worlds[0].Views[0].Eye.Get_Y_Angle()
		<< "|" << Camera_Speed << "|" << Snap << std::endl;
	for (int i = 0; i < Indigo::Worlds[0].Number_Of_Objects(); ++i)
	{
		Object& object = Indigo::Worlds[0].Get_Object(i);
		if (object.Data.Size != 0)
		{
			file << object.X << "|" << object.Y << "|" << object.Z << "|" << object.Facing.Get_X_Angle() << "|" << object.Facing.Get_Y_Angle()
				<< "|" << object.Color.r << "|" << object.Color.g << "|" << object.Color.b << "|" << object.Color.a << "|" << object.Shine
				<< "|" << Objs[i] << "|" << Textures[i] << "|" << Bump_Maps[i] << std::endl;
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
	file << "Indigo::Worlds[0].View.Place(" << Indigo::Worlds[0].Views[0].X << ", " << Indigo::Worlds[0].Views[0].Y << ", " << Indigo::Worlds[0].Views[0].Z << ");" << std::endl;
	file << "Indigo::Worlds[0].View.Eye.Set_Direction(1, " << Indigo::Worlds[0].Views[0].Eye.Get_X_Angle() << ", " << Indigo::Worlds[0].Views[0].Eye.Get_Y_Angle() << ");" << std::endl;
	for (int i = 0; i < Indigo::Worlds[0].Number_Of_Objects(); ++i)
	{
		Object& object = Indigo::Worlds[0].Get_Object(i);
		if (object.Data.Size != 0)
		{
			file << "Indigo::Worlds[0].Add_Object(Object(" << object.X << ", " << object.Y << ", " << object.Z
				<< ", Mesh(" << Objs[i] << ", " << Textures[i] << ", " << Bump_Maps[i] << "), nullptr, " << object.Shine << "Direction(1, "
				<< object.Facing.Get_X_Angle() << ", " << object.Facing.Get_Y_Angle() << "), glm::vec4("
				<< object.Color.r << ", " << object.Color.g << ", " << object.Color.b << ", " << object.Color.a << ")));" << std::endl;
		}
	}
	return true;
}

bool Add_And_Save(glm::vec3& position, const std::string& mesh, const std::string& texture, const std::string& bump_map)
{
	Mesh add = Mesh(mesh.c_str(), texture.length() ? texture.c_str() : nullptr, bump_map.length() ? bump_map.c_str() : nullptr);
	if (add.Size == 0)
	{
		return false;
	}
	Objs.push_back(mesh);
	Textures.push_back(texture);
	Bump_Maps.push_back(bump_map);
	Indigo::Worlds[0].Add_Object(Object(position.x, position.y, position.z, add));
	return true;
}

void Set_Wireframe(void)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}
void Stop_Wireframe(void)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Toggle_Select(Object * object)
{
	To_Move->Pre_Render_Function = nullptr;
	To_Move->Post_Render_Function = nullptr;
	if (object != To_Move)
	{
		// Selecting new
		object->Pre_Render_Function = Set_Wireframe;
		object->Post_Render_Function = Stop_Wireframe;
		Undo_Index = object->ID;
		Undo_Object = *object;
		To_Move = object;
	}
	else
	{
		// Double select: Deselect
		To_Move = &Motion;
	}
}

void Mouse_Interact(int button, int state, float x, float y)
{
	if (state == GLFW_RELEASE)
	{
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			int collide = Indigo::Worlds[0].Collide(Indigo::Mouse_Position);
			if (collide != -1)
			{
				Object * clicked = &Indigo::Worlds[0].Get_Object(collide);
				Toggle_Select(clicked);
			}
			else
			{
				Toggle_Select(To_Move);
			}
		}
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (Current_Menu != -1)
			{
				Object& menu = Indigo::Worlds[0].Get_2D_Object(Current_Menu);
				Cursor_Pos = int((x - menu.X) / 0.05 + 0.5);
				if (Cursor_Pos < 0)
					Cursor_Pos = 0;
				if (Cursor_Pos > Typing.length())
					Cursor_Pos = Typing.length();
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
		static float cancel = false;
		if (!cancel)
		{
			Motion.Facing.Add_Direction(0.0, x*0.2, y*-0.2);
			if (Motion.Facing.Get_Y_Angle() > 90 && Motion.Facing.Get_Y_Angle() < 270)
			{
				Indigo::Worlds[0].Views[0].Up.Set_Y(-1.0);
			}
			else
			{
				Indigo::Worlds[0].Views[0].Up.Set_Y(1.0);
			}
		}
		cancel = true;
		if (Indigo::Mouse_Position_Screen.x > Indigo::Screen_Width - 10)
			glfwSetCursorPos(Indigo::Window, 10, Indigo::Mouse_Position_Screen.y);
		else if (Indigo::Mouse_Position_Screen.x < 10)
			glfwSetCursorPos(Indigo::Window, Indigo::Screen_Width - 10, Indigo::Mouse_Position_Screen.y);
		else if (Indigo::Mouse_Position_Screen.y > Indigo::Screen_Height - 10)
			glfwSetCursorPos(Indigo::Window, Indigo::Mouse_Position_Screen.x, 10);
		else if (Indigo::Mouse_Position_Screen.y < 10)
			glfwSetCursorPos(Indigo::Window, Indigo::Mouse_Position_Screen.x, Indigo::Screen_Height - 10);
		else
			cancel = false;
	}
}

void GUI(float time)
{
	static int cursor_object = -1;
	if (cursor_object == -1)
	{
		cursor_object = Indigo::Worlds[0].Add_2D_Object(Object(0, 0, 0, Mesh::Text("|", 0.06, "Textures/Font.png", glm::vec4(0, 0, 0, 1))));
	}
	if (Current_Menu == -1)
	{
		if (Indigo::Keys['2'])
		{
			Camera_Speed += 1 * Camera_Speed * time;
		}
		if (Indigo::Keys['1'])
		{
			Camera_Speed -= 1 * Camera_Speed * time;
		}
		const float speed = Camera_Speed * time;
		Direction stored_facing = To_Move->Facing;
		if (To_Move != &Motion)
		{
			// Objects move away from the camera on W, left on A, etc, but only to the nearest right angle
			To_Move->Facing = Direction(1, int((Motion.Facing.Get_X_Angle() + 45) / 90) * 90 + To_Move->Facing.Get_X_Angle(), int((To_Move->Facing.Get_Y_Angle() + 45) / 90) * 90);
		}
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
		To_Move->Facing = stored_facing;
		if (!Indigo::Keys['w'] && !Indigo::Keys['s'] && !Indigo::Keys['a'] && !Indigo::Keys['d'] && !Indigo::Keys['e'] && !Indigo::Keys['q'])
		{
			// No key pressed
			if (Snap && To_Move != &Motion)
			{
				To_Move->X = int((To_Move->X + Snap / 2 * (To_Move->X > 0 ? 1 : -1)) / Snap) * Snap;
				To_Move->Y = int((To_Move->Y + Snap / 2 * (To_Move->Y > 0 ? 1 : -1)) / Snap) * Snap;
				To_Move->Z = int((To_Move->Z + Snap / 2 * (To_Move->Z > 0 ? 1 : -1)) / Snap) * Snap;
			}
		}
		if (Position_Marker == -1)
		{
			Position_Marker = Indigo::Worlds[0].Add_2D_Object(Object());
		}
		Object& marker = Indigo::Worlds[0].Get_2D_Object(Position_Marker);
		if (Indigo::Elapsed() > Splash_Time && Render_Marker)
		{
			std::string x = std::to_string(To_Move->X);
			std::string y = std::to_string(To_Move->Y);
			std::string z = std::to_string(To_Move->Z);
			std::string speed = std::to_string(Camera_Speed);
			std::string display = "Indigo Engine Level Designer\n"
				"Position  " + x.substr(0, x.length() - 4) + "  " + y.substr(0, y.length() - 4) + "  " + z.substr(0, z.length() - 4) + "\n"
				+ "FPS  " + std::to_string(Indigo::Actual_FPS) + "  MPF  " + std::to_string(int(time * 1000)) + "\n"
				+ "Camera Speed  " + speed.substr(0, speed.length() - 5) + "  M/S" "\n"
				+ "Objects  " + std::to_string(Indigo::Worlds[0].Number_Of_Objects()) + "\n"
				"Press H for help";
			marker.Data = Mesh::Text(display.c_str(), 0.05, "Textures/Font.png", glm::vec4(0, 0, 0, 1), glm::vec4(Indigo::Sky_Color, 0.9));
			marker.X = -1 * Indigo::Aspect_Ratio + 0.1;
			marker.Y = -0.9 - marker.Data.Hitbox[0].y;
		}
		else
		{
			marker.Data = Mesh();
		}
		Indigo::Worlds[0].Get_2D_Object(cursor_object).X = -100;
	}
	else
	{
		if (cursor_object < Current_Menu) // The cursor must be always on top
		{
			Indigo::Worlds[0].Remove_2D_Object(cursor_object);
			cursor_object = Indigo::Worlds[0].Add_2D_Object(Object(0, 0, 0, Mesh::Text("|", 0.06, "Textures/Font.png", glm::vec4(0, 0, 0, 1))));
		}
		Cursor_Delay += time;
		Object& marker = Indigo::Worlds[0].Get_2D_Object(cursor_object);
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
			Object& menu = Indigo::Worlds[0].Get_2D_Object(Current_Menu);
			marker.X = menu.X + Cursor_Pos * 0.05 - 0.0075;
			marker.Y = menu.Y + menu.Data.Hitbox[0].y - ((Typing.length() == 0) ? 0.05 : 0);
		}
	}
	Indigo::Worlds[0].Views[0].Place(Motion.X, Motion.Y, Motion.Z);
	Indigo::Worlds[0].Views[0].Eye = Motion.Facing;
}

void Key_Pressed(int key)
{

	// Each menu that can be triggered by a key must have a handle
	static int space_menu = -1;
	static int save_menu = -1;
	static int open_menu = -1;
	static int position_menu = -1;
	static int facing_menu = -1;
	static int snap_menu = -1;
	static int color_menu = -1;

	// Some menus have state variables that must be preserved
	static int menu_stage = 0;

	// Menus spawn near the mouse. Store where.
	static float menu_x = 0;
	static float menu_y = 0;

	// User input may be remembered for easier input in the future
	static std::string save_location = "";
	static std::string mesh = "";
	static std::string texture = "";
	static std::string bump_map = "";

	Cursor_On = true;
	Cursor_Delay = 0;

	// Space Menu adds Models to the World
	if (space_menu != -1)
	{
		bool changed = false;
		if (key == GLFW_KEY_ESCAPE && Typing.length() == 0)
		{
			if (menu_stage > 0)
			{
				menu_stage -= 1;
				changed = true;
				if (menu_stage == 2)
				{
					Typing = texture;
				}
				else
				{
					Typing = mesh;
				}
			}
			else
			{
				Indigo::Worlds[0].Remove_2D_Object(space_menu);
				space_menu = -1;
				Current_Menu = -1;
			}
		}
		if ((key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER))
		{
			if (menu_stage == 2)
			{
				Print(space_menu, "Loading...");
				Indigo::Render();
				bump_map = Typing;
				glm::vec3 position = glm::vec3(Indigo::Worlds[0].Views[0].X, Indigo::Worlds[0].Views[0].Y, Indigo::Worlds[0].Views[0].Z);
				if (Add_And_Save(position, mesh, texture, bump_map))
				{
					Undo_Index = Indigo::Worlds[0].Number_Of_Objects() - 1;
					Undo_Object = Object();
					To_Move = &Motion; // Random one because we'll toggle select next line
					Toggle_Select(&Indigo::Worlds[0].Get_Object(Undo_Index));
					Indigo::Worlds[0].Remove_2D_Object(space_menu);
					space_menu = -1;
					Current_Menu = -1;
				}
				else
				{
					Indigo::Worlds[0].Add_2D_Object(Object(menu_x, menu_y + 0.05, 0, Mesh::Text("Couldn't find / load OBJ.", 0.05, "Textures/Font.png", glm::vec4(0,0,0,1), glm::vec4(Indigo::Sky_Color, 0.9)), Fade_Text));
					menu_stage = 0;
					Typing = mesh;
					changed = true;
				}
			}
			else if (menu_stage == 1)
			{
				texture = Typing;
				menu_stage = 2;
				if (bump_map == "")
				{
					bump_map = texture.substr(0, mesh.length() - 4) + "_Bump.png";
				}
				Typing = bump_map;
				Cursor_Pos = Typing.length();
				changed = true;
			}
			else
			{
				mesh = Typing;
				if (!mesh.length())
				{
					Indigo::Worlds[0].Remove_2D_Object(space_menu);
					space_menu = -1;
					Current_Menu = -1;
					mesh = "";
				}
				else
				{
					menu_stage = 1;
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
			if (changed)
			{
				// Only if it was a standard character
				if (menu_stage == 0)
				{
					texture = "";
					bump_map = "";
				}
				if (menu_stage == 1)
				{
					bump_map = "";
				}
			}
		}
		if (changed)
		{
			if (menu_stage == 0)
				Print(space_menu, "Mesh:\n" + Typing);
			if (menu_stage == 1)
				Print(space_menu, "Texture:\n" + Typing);
			if (menu_stage == 2)
				Print(space_menu, "Bump Map:\n" + Typing);
		}
	}

	// Save Menu saves the current World
	else if (save_menu != -1)
	{
		if (key == GLFW_KEY_ESCAPE && Typing.length() == 0)
		{
			Indigo::Worlds[0].Remove_2D_Object(save_menu);
			save_menu = -1;
			Current_Menu = -1;
		}
		if ((key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER))
		{
			Print(save_menu, "Loading...");
			Indigo::Render();
			bool success;
			save_location = Typing;
			if (menu_stage == 1)
			{
				success = Save_For_Compile(Typing.c_str());
			}
			else
			{
				success = Save(Typing.c_str());
			}
			if (success)
			{
				Indigo::Worlds[0].Remove_2D_Object(save_menu);
				save_menu = -1;
				Current_Menu = -1;
			}
			else
			{
				Indigo::Worlds[0].Add_2D_Object(Object(menu_x, menu_y + 0.05, 0, Mesh::Text("Unable to save file. Check for write permissions.", 0.05, "Texture/Font.png", glm::vec4(0, 0, 0, 1), glm::vec4(Indigo::Sky_Color, 0.9)), Fade_Text));
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
			Indigo::Worlds[0].Remove_2D_Object(open_menu);
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
				Indigo::Worlds[0].Remove_2D_Object(open_menu);
				open_menu = -1;
				Current_Menu = -1;
			}
			else
			{
				Indigo::Worlds[0].Add_2D_Object(Object(menu_x, menu_y + 0.05, 0, Mesh::Text("Unable to open file.", 0.05, "Textures/Font.png", glm::vec4(0, 0, 0, 1), glm::vec4(Indigo::Sky_Color, 0.9)), Fade_Text));
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
			if (menu_stage > 0)
			{
				menu_stage--;
			}
			else
			{
				Indigo::Worlds[0].Remove_2D_Object(position_menu);
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
			if (menu_stage == 0)
			{
				To_Move->X = Indigo::Fast_Float(Typing.c_str());
				Typing = std::to_string(To_Move->Y);
				Typing = Typing.substr(0, Typing.length() - 4);
			}
			if (menu_stage == 1)
			{
				To_Move->Y = Indigo::Fast_Float(Typing.c_str());
				Typing = std::to_string(To_Move->Z);
				Typing = Typing.substr(0, Typing.length() - 4);
			}
			if (menu_stage == 2)
			{
				To_Move->Z = Indigo::Fast_Float(Typing.c_str());
				Indigo::Worlds[0].Remove_2D_Object(position_menu);
				position_menu = -1;
				Current_Menu = -1;
				changed = false;
			}
			menu_stage++;
			Cursor_Pos = Typing.length();
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
			if (menu_stage == 0)
				which = "X:\n";
			if (menu_stage == 1)
				which = "Y:\n";
			if (menu_stage == 2)
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
			if (menu_stage > 0)
			{
				menu_stage--;
			}
			else
			{
				Indigo::Worlds[0].Remove_2D_Object(facing_menu);
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
			if (menu_stage == 0)
			{
				To_Move->Facing.Set_X_Angle(Indigo::Fast_Float(Typing.c_str()));
				Typing = std::to_string(To_Move->Facing.Get_Y_Angle());
				Typing = Typing.substr(0, Typing.length() - 4);
			}
			if (menu_stage == 1)
			{
				To_Move->Facing.Set_Y_Angle(Indigo::Fast_Float(Typing.c_str()));
				Indigo::Worlds[0].Remove_2D_Object(facing_menu);
				facing_menu = -1;
				Current_Menu = -1;
				changed = false;
			}
			menu_stage++;
			Cursor_Pos = Typing.length();
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
			if (menu_stage == 0)
				which = "X Angle:\n";
			if (menu_stage == 1)
				which = "Y Angle:\n";
			Print(facing_menu, which + Typing);
		}
	}
	
	// Snap menu allows each object to always be at a position of a multiple of Snap
	else if (snap_menu != -1)
	{

		if (key == GLFW_KEY_ESCAPE && Typing.length() == 0)
		{
			Indigo::Worlds[0].Remove_2D_Object(snap_menu);
			snap_menu = -1;
			Current_Menu = -1;
		}
		if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER)
		{
			Snap = Indigo::Fast_Float(Typing.c_str());
			Indigo::Worlds[0].Remove_2D_Object(snap_menu);
			snap_menu = -1;
			Current_Menu = -1;
		}
		else if (Text_Edit(key, Typing))
		{
			Print(snap_menu, "Snap to nearest:\n" + Typing);
		}
	}

	else if (color_menu != -1)
	{

		bool changed = true;
		if (key == GLFW_KEY_ESCAPE && Typing.length() == 0)
		{
			if (menu_stage > 0)
			{
				menu_stage--;
			}
			else
			{
				Indigo::Worlds[0].Remove_2D_Object(color_menu);
				color_menu = -1;
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
			if (menu_stage == 0)
			{
				// Red
				To_Move->Color.r = Indigo::Fast_Float(Typing.c_str());
				Typing = std::to_string(To_Move->Color.g);
				Typing = Typing.substr(0, Typing.length() - 3);
			}
			if (menu_stage == 1)
			{
				// Green
				To_Move->Color.g = Indigo::Fast_Float(Typing.c_str());
				Typing = std::to_string(To_Move->Color.b);
				Typing = Typing.substr(0, Typing.length() - 3);
			}
			if (menu_stage == 2)
			{
				// Blue
				To_Move->Color.b = Indigo::Fast_Float(Typing.c_str());
				Typing = std::to_string(To_Move->Color.a);
				Typing = Typing.substr(0, Typing.length() - 3);
			}
			if (menu_stage == 3)
			{
				// Alpha
				To_Move->Color.a = Indigo::Fast_Float(Typing.c_str());
				Typing = std::to_string(To_Move->Shine);
				Typing = Typing.substr(0, Typing.length() - 3);
			}
			if (menu_stage == 4)
			{
				// Shine
				To_Move->Shine = Indigo::Fast_Float(Typing.c_str());
				Indigo::Worlds[0].Remove_2D_Object(color_menu);
				color_menu = -1;
				Current_Menu = -1;
				changed = false;
			}
			menu_stage++;
			Cursor_Pos = Typing.length();
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
			if (menu_stage == 0)
				which = "Red:\n";
			if (menu_stage == 1)
				which = "Green:\n";
			if (menu_stage == 2)
				which = "Blue:\n";
			if (menu_stage == 3)
				which = "Opacity:\n";
			if (menu_stage == 4)
				which = "Shininess:\n";
			Print(color_menu, which + Typing);
		}

	}

	// These keys add menus
	else
	{
		if (key == ' ' || (Indigo::Control && key == 'a')) // Add objects
		{

			menu_x = Indigo::Mouse_Position.x + 0.07;
			menu_y = Indigo::Mouse_Position.y - 0.07;

#ifdef _WIN32

			// We're on windows! Use the file dialogue, it's WAY more convenient

			mesh = "";
			texture = "";
			bump_map = "";

			OPENFILENAME dialogue;

			memset(&dialogue, 0, sizeof(OPENFILENAME));

			wchar_t name[MAX_PATH];
			*name = 0;

			dialogue.lStructSize       = sizeof(OPENFILENAME);
			dialogue.hwndOwner         = GetFocus();
			dialogue.lpstrFilter       = (LPCWSTR) L"Obj Models\0*.obj\0";
			dialogue.lpstrFile         = (LPWSTR) name;
			dialogue.nMaxFile          = MAX_PATH;
			dialogue.lpstrInitialDir   = (LPWSTR) L".";
			dialogue.nMaxFileTitle     = MAX_PATH;
			dialogue.lpstrTitle        = (LPWSTR) L"Mesh";

			dialogue.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			if (!GetOpenFileName((LPOPENFILENAME) &dialogue))
			{
				// Almost definitely canceled
				return;
			}
			else
			{
				for (int i = 0; name[i] != '\0'; ++i)
				{
					mesh += name[i];
				}
			}

			dialogue.lpstrFilter = (LPCWSTR) L"Supported Images\0*.jpg;*.jpeg;*.png;*.tga;*.bmp;*.psd;*.gif;*.hdr;*.pic\0";
			dialogue.lpstrTitle  = (LPCWSTR) L"Texture";

			if (!GetOpenFileName((LPOPENFILENAME) &dialogue))
			{
				// A cancel now means, "No, Thanks." Simply load blank texture.
				texture = "";
			}
			else
			{
				for (int i = 0; name[i] != '\0'; ++i)
				{
					texture += name[i];
				}
			}

			dialogue.lpstrTitle = (LPCWSTR) L"Bump Map";

			if (!GetOpenFileName((LPOPENFILENAME) &dialogue))
			{
				// A cancel now means, "No, Thanks." Simply load blank texture.
				bump_map = "";
			}
			else
			{
				for (int i = 0; name[i] != '\0'; ++i)
				{
					bump_map += name[i];
				}
			}

			int loading = Indigo::Worlds[0].Add_2D_Object(Object(menu_x, menu_y));
			Print(loading, "Loading...");
			Indigo::Render();
			glm::vec3 position = glm::vec3(Indigo::Worlds[0].Views[0].X, Indigo::Worlds[0].Views[0].Y, Indigo::Worlds[0].Views[0].Z);
			if (Add_And_Save(position, mesh, texture, bump_map))
			{
				Undo_Index = Indigo::Worlds[0].Number_Of_Objects() - 1;
				Undo_Object = Object();
				To_Move = &Motion;
				Toggle_Select(&Indigo::Worlds[0].Get_Object(Undo_Index));
				Indigo::Worlds[0].Remove_2D_Object(loading);
			}
			else
			{
				Print(loading, "Failed to load.");
				Indigo::Worlds[0].Get_2D_Object(loading).Update_Function = Fade_Text;
			}
			Indigo::Keys[' '] = false;

#else

			// Other system: use custom OpenGL file select
			space_menu = Indigo::Worlds[0].Add_2D_Object(Object(menu_x, menu_y));
			Typing = mesh;
			std::string display = "Mesh:\n" + Typing;
			Current_Menu = space_menu;
			Print(space_menu, display);

#endif

		}
		if (key == GLFW_KEY_TAB) // Position
		{
			Undo_Index = To_Move->ID;
			Undo_Object = *To_Move;
			menu_x = Indigo::Mouse_Position.x + 0.07;
			menu_y = Indigo::Mouse_Position.y - 0.07;
			Typing = std::to_string(To_Move->X);
			Typing = Typing.substr(0, Typing.length() - 4);
			std::string display = "X:\n" + Typing;
			position_menu = Indigo::Worlds[0].Add_2D_Object(Object(menu_x, menu_y));
			Current_Menu = position_menu;
			Print(position_menu, display);
		}
		if (key == 'f') // Facing
		{
			Undo_Index = To_Move->ID;
			Undo_Object = *To_Move;
			menu_x = Indigo::Mouse_Position.x + 0.07;
			menu_y = Indigo::Mouse_Position.y - 0.07;
			Typing = std::to_string(To_Move->Facing.Get_X_Angle());
			Typing = Typing.substr(0, Typing.length() - 4);
			std::string display = "X Angle:\n" + Typing;
			facing_menu = Indigo::Worlds[0].Add_2D_Object(Object(menu_x, menu_y));
			Current_Menu = facing_menu;
			Print(facing_menu, display);
		}
		if (key == 'g') // snap to Grid
		{
			menu_x = Indigo::Mouse_Position.x + 0.07;
			menu_y = Indigo::Mouse_Position.y - 0.07;
			Typing = "";
			std::string display = "Snap to nearest:\n";
			snap_menu = Indigo::Worlds[0].Add_2D_Object(Object(menu_x, menu_y));
			Current_Menu = snap_menu;
			Print(snap_menu, display);
		}
		if (key == GLFW_KEY_BACKSPACE || key == GLFW_KEY_DELETE || key == 'x') // Delete
		{
			if (To_Move != &Motion)
			{
				Undo_Index = To_Move->ID;
				Undo_Object = *To_Move;
				Indigo::Worlds[0].Remove_Object(To_Move->ID);
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
		if (key == 'f') // Fall toggle
		{
			Physics = !Physics;
		}
		if (key == 'm') // Material options
		{
			menu_x = Indigo::Mouse_Position.x + 0.07;
			menu_y = Indigo::Mouse_Position.y - 0.07;
			Typing = std::to_string(To_Move->Color.r);
			Typing = Typing.substr(0, Typing.length() - 3);
			std::string display = "Red:\n" + Typing;
			color_menu = Indigo::Worlds[0].Add_2D_Object(Object(menu_x, menu_y));
			Current_Menu = color_menu;
			Print(color_menu, display);
		}
		if (Indigo::Control)
		{
			if (key == 's') // Save
			{
				menu_x = Indigo::Mouse_Position.x + 0.07;
				menu_y = Indigo::Mouse_Position.y - 0.07;

#ifdef _WIN32

				// Windows dialogue

				OPENFILENAME dialogue;

				memset(&dialogue, 0, sizeof(OPENFILENAME));

				wchar_t name[MAX_PATH];
				*name = 0;

				dialogue.lStructSize = sizeof(OPENFILENAME);
				dialogue.hwndOwner = GetFocus();
				dialogue.lpstrFilter = (LPCWSTR) L"IELD Level\0*.ild;*.ield;*.*\0";
				dialogue.lpstrFile = (LPWSTR) name;
				dialogue.nMaxFile = MAX_PATH;
				dialogue.lpstrInitialDir = (LPWSTR) L".";
				dialogue.nMaxFileTitle = MAX_PATH;
				dialogue.lpstrTitle = (LPWSTR) L"Save Level";
				dialogue.lpstrDefExt = (LPWSTR) L"ild";

				dialogue.Flags = OFN_NOCHANGEDIR;

				std::string location = "";

				if (!GetOpenFileName((LPOPENFILENAME) &dialogue))
				{
					// Almost definitely canceled
					return;
				}
				else
				{
					for (int i = 0; name[i] != '\0'; ++i)
					{
						location += name[i];
					}
				}

				int loading = Indigo::Worlds[0].Add_2D_Object(Object(menu_x, menu_y));
				Print(loading, "Loading...");
				Indigo::Render();
				if (Indigo::Shift)
				{
					Save_For_Compile(location.c_str());
				}
				else
				{
					Save(location.c_str());
				}
				Indigo::Worlds[0].Remove_2D_Object(loading);
				Indigo::Keys['s'] = false;

#else

				Typing = save_location;
				std::string display = "Save Here:\n" + Typing;
				save_menu = Indigo::Worlds[0].Add_2D_Object(Object(menu_x, menu_y));
				Current_Menu = save_menu;
				Print(save_menu, display);
				menu_stage = Indigo::Shift;

#endif

			}
			if (key == 'o') // Open
			{

				Global_Values();
				Indigo::Worlds[0] = Restore;

				menu_x = Indigo::Mouse_Position.x + 0.07;
				menu_y = Indigo::Mouse_Position.y - 0.07;

#ifdef _WIN32

				// Windows dialogue

				OPENFILENAME dialogue;

				memset(&dialogue, 0, sizeof(OPENFILENAME));

				wchar_t name[MAX_PATH];
				*name = 0;

				dialogue.lStructSize = sizeof(OPENFILENAME);
				dialogue.hwndOwner = GetFocus();
				dialogue.lpstrFilter = (LPCWSTR) L"IELD Levels\0*.ild;*.ield;*.*\0";
				dialogue.lpstrFile = (LPWSTR) name;
				dialogue.nMaxFile = MAX_PATH;
				dialogue.lpstrInitialDir = (LPWSTR) L".";
				dialogue.nMaxFileTitle = MAX_PATH;
				dialogue.lpstrTitle = (LPWSTR) L"Open Level";

				dialogue.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

				std::string location = "";

				if (!GetOpenFileName((LPOPENFILENAME) &dialogue))
				{
					// Almost definitely canceled
					return;
				}
				else
				{
					for (int i = 0; name[i] != '\0'; ++i)
					{
						location += name[i];
					}
				}

				int loading = Indigo::Worlds[0].Add_2D_Object(Object(menu_x, menu_y));
				Print(loading, "Loading...");
				Indigo::Render();
				Load(location.c_str());
				Indigo::Worlds[0].Remove_2D_Object(loading);
				Indigo::Keys['o'] = false;

#else

				// Standard method
				Typing = save_location;
				std::string display = "Open From:\n" + Typing;
				open_menu = Indigo::Worlds[0].Add_2D_Object(Object(menu_x, menu_y));
				Current_Menu = open_menu;
				Print(open_menu, display);

#endif

			}
			if (key == 'n') // New
			{
				Global_Values();
				Indigo::Worlds[0] = Restore;
			}
			if (key == 'c') // Copy
			{
				if (To_Move != &Motion)
				{
					Copied = To_Move->ID;
				}
			}
			if (key == 'v') // Paste
			{
				if (Copied != -1)
				{
					int place = Indigo::Worlds[0].Add_Object(Indigo::Worlds[0].Get_Object(Copied));
					To_Move = &Indigo::Worlds[0].Get_Object(Copied); // All object references are invalidated when things are added
					Object& pasted = Indigo::Worlds[0].Get_Object(place);
					pasted.Place(Motion.X, Motion.Y, Motion.Z);
					Toggle_Select(&pasted);
					Objs.push_back(Objs[Copied]);
					Textures.push_back(Textures[Copied]);
					Bump_Maps.push_back(Bump_Maps[Copied]);
				}
			}
			if (key == 'z') // Undo
			{
				if (Undo_Index != -1)
				{
					Object undo = Indigo::Worlds[0].Get_Object(Undo_Index);
					Indigo::Worlds[0].Get_Object(Undo_Index) = Undo_Object;
					Toggle_Select(&undo); // In order to have done something to the object it was selected
					// Undo the undo
					Undo_Object = undo;
				}
			}
		}
		Cursor_Pos = Typing.length();
		if (!(Indigo::Control && key == 's'))
		{
			menu_stage = 0;
		}
	}
}

std::map<char *, char *> Speeches;
std::map<char *, std::vector<char *>> Responses;

void Initialize_Speech()
{

	Speeches["Hi"] = "Hello, how are you?";
	Speeches["Good"] = "I'm doing fancy, thank you!";
	Speeches["Bad"] = "I am doing terribly.";
	Speeches["Agree Good"] = "Yeah I'm doing well, too. Thank you.";
	Speeches["Agree Bad"] = "Me too. I hate this darned boat. It's the damn creaking, isn't it?";
	Speeches["Ok"] = "Ok. Bye.";
	Speeches["Bye"] = "Yeah. Bye. See you around.";

	Responses["Hi"].push_back("Good");
	Responses["Hi"].push_back("Bad");
	Responses["Bad"].push_back("Agree Bad");
	Responses["Good"].push_back("Agree Good");
	Responses["Agree Bad"].push_back("Ok");
	Responses["Agree Good"].push_back("Ok");

}

void Global_Values(void)
{
	Motion = Object();
	To_Move = &Motion;
	Objs = std::vector<std::string>();
	Textures = std::vector<std::string>();
	Bump_Maps = std::vector<std::string>();
	Camera_Speed = 5;
	Physics = false;
	Position_Marker = -1;
	Render_Marker = true;
	Snap = 0;
	Undo_Index = -1;
	Undo_Object = Object();
	Copied = -1;
	Typing = "";
	Current_Menu = -1;
	Cursor_Pos = 0;
	Cursor_Delay = 0;
	Cursor_On = false;
}

int main(int argc, char ** argv)
{
	Indigo::Initialize("Indigo Engine Level Designer", Indigo::Sky_Color, 1, 24, true, -240, -135, false);
	Global_Values();
	Indigo::Worlds[0].Update_Function = GUI;
	Indigo::Worlds[0].Views[0].Shader("Indigo/Shaders/Default.vs", "Indigo/Shaders/Default.fs");
	Indigo::Worlds[0].Light_Setup.Set_Ambient(0.075);
	Indigo::Worlds[0].Light_Setup.Add_Sun(0, -1, 0);
	//Indigo::Worlds[0].Light_Setup.Add_Bulb(0, 1.88, 0, 0.5);
	//Indigo::Worlds[0].Light_Setup.Add_Lamp(-0.35662, 0.6792, 1.92358, glm::vec3(-1, -1, 0), 90, 0.4);
	Indigo::Mouse_Button_Function = Mouse_Interact;
	Indigo::Relative_Mouse_Moved_Function = Mouse_Look;
	Indigo::Key_Pressed_Function = Key_Pressed;
	Indigo::Key_Repeat_Function = Key_Pressed;
	Restore = Indigo::Worlds[0];
	//Indigo::Construct_Splash

	//Initialize_Speech();

	/*char * key = "Hi";
	std::cout << Speeches[key] << std::endl;
	while (Responses[key].size())
	{
		std::cout << std::endl << "Possible responses:" << std::endl;
		for (int i = 0; i < Responses[key].size(); ++i)
		{
			std::cout << i << ": " << Speeches[Responses[key][i]] << std::endl;
		}
		std::cout << std::endl << "Your response: ";
		int response;
		std::cin >> response;
		key = Responses[key][response]; // The key is now the character's response (we don't need to print that)
		if (Responses[key].size())
		{
			key = Responses[key][0]; // The key is now the AI's reponse (we need to print it)
			std::cout << std::endl << Speeches[key] << std::endl;
		}
	}*/

	Indigo::Worlds[0].Add_2D_Object(Object(0, 0, 0, Mesh::Rectangle(1, 1), Indigo::Splash_Screen));

	if (argc > 1)
	{
		Load(argv[1]);
	}
	Indigo::Run();
}
