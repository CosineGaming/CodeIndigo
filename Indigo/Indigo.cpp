// Basic OpenGL functions, also takes care of most of the background work.
// These function will mostly be called in Main.

#include "GL/glew.h"

#include "Indigo.h"

#include <string>


namespace Indigo
{
	// Initializes window and rendering matrices. Really ought to be called first.
	void Initialize(const char * window_name, glm::vec3 background, const int aa_samples,
		const int max_framerate, const int window_width, const int window_height, const bool fullscreen)
	{
		// Initiate glut
		glfwSetErrorCallback(Error_Found);
		glfwInit();
		glfwWindowHint(GLFW_SAMPLES, aa_samples);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		int width = window_width;
		int height = window_height;
		if (window_width < 1 || window_height < 1)
		{
			const GLFWvidmode * monitor = glfwGetVideoMode(glfwGetPrimaryMonitor());
			if (window_width < 1)
			{
				width += monitor->width;
			}
			if (window_height < 1)
			{
				height += monitor->height;
			}
		}
		Window = glfwCreateWindow(width, height, window_name, fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
		glfwSetFramebufferSizeCallback(Window, Reshape);
		glfwSetCursorPosCallback(Window, Mouse_Moved);
		glfwSetMouseButtonCallback(Window, Mouse_Button);
		glfwSetKeyCallback(Window, Key_Action);
		glfwMakeContextCurrent(Window);

		// GLEW does all this magical driver OGL stuff
		glewExperimental = GL_TRUE;
		if (glewInit())
		{
			std::cout << "There was this GLEW thing, and it was all like, NO!" << std::endl << "And we were sad. Try reinstalling."
				<< std::endl << "For more info contact the creator of your game." << std::endl;
		}

		glClearColor(background[0], background[1], background[2], 1.0);
		if (max_framerate == 0)
		{
			Frame_Length_Minimum = 0;
		}
		else
		{
			Frame_Length_Minimum = 1000.0 / max_framerate;
		}

		//glGenVertexArrays(1, &VAO);
		//glBindVertexArray(VAO);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_DITHER);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// Setup fog
		//glEnable(GL_FOG);
		//glFogfv(GL_FOG_COLOR, White_Color);
		//glHint(GL_FOG_HINT, GL_DONT_CARE);
		//glFogf(GL_FOG_DENSITY, 0.002);

		// Enable rendering options
		Reshape(Window);

		//Indigo::Error_Dump();

		return;
	}

	// Starts the main loop with update, render, and input
	int Run(void)
	{
		Render();
		glfwSwapBuffers(Indigo::Window);
		glfwPollEvents();
		float last = Elapsed();
		while (!glfwWindowShouldClose(Window))
		{
			float now = Elapsed();
			float delta = now - last;
			if (delta >= Frame_Length_Minimum)
			{
				last = now;
				Update(delta);
				Render();
			}
		}
		glfwTerminate();
		return 0;
	}

	void Close(void)
	{
		glfwSetWindowShouldClose(Window, true);
		return;
	}

	// Acts for when the window reshapes
	void Reshape(GLFWwindow * window, int width, int height)
	{
		int window_width;
		int window_height;
		if (0 == width || 0 == height)
		{
			glfwGetFramebufferSize(Window, &window_width, &window_height);
		}
		else
		{
			window_width = width;
			window_height = height;
			glViewport(0, 0, width, height);
		}
		Screen_Width = window_width;
		Screen_Height = window_height;
		Aspect_Ratio = (float) window_width / (float) window_height;
	}

	// Acts for Keys which act once, and stores for multi-acting Keys
	void Key_Action(GLFWwindow * window, int key, int scancode, int action, int modifiers)
	{
		if (key >= 'A' && key <= 'Z')
		{
			key += 'a' - 'A';
		}

		Shift = (modifiers & GLFW_MOD_SHIFT) != 0;
		Control = (modifiers & GLFW_MOD_CONTROL) != 0;
		Alt = (modifiers & GLFW_MOD_ALT) != 0;

		if (action == GLFW_PRESS)
		{
			if (Key_Pressed_Function)
			{
				Key_Pressed_Function(key);
			}
			Keys[key] = true;
			Keys_Pressed[key] = true;
		}
		if (action == GLFW_RELEASE)
		{
			if (Key_Released_Function)
			{
				Key_Released_Function(key);
			}
			Keys[key] = false;
		}
		return;
	}

	// Checks whether a key has just been pressed
	bool Pressed(unsigned char key)
	{
		bool pressed = Keys_Pressed[key];
		Keys_Pressed[key] = false;
		return pressed;
	}

	// Acts for when the mouse is pressed or released
	void Mouse_Button(GLFWwindow * window, int button, int state, int modifiers)
	{
		Shift = (modifiers & GLFW_MOD_SHIFT) != 0;
		Control = (modifiers & GLFW_MOD_CONTROL) != 0;
		Alt = (modifiers & GLFW_MOD_ALT) != 0;
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			Left_Mouse = state == GLFW_PRESS;
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			Right_Mouse = state == GLFW_PRESS;
		}
		if (button == GLFW_MOUSE_BUTTON_MIDDLE)
		{
			Middle_Mouse = state == GLFW_PRESS;
		}
		if (Mouse_Button_Function)
		{
			Mouse_Button_Function(button, state, Mouse_Position.x, Mouse_Position.y);
		}
		return;
	}

	// Acts for when the mouse is moved
	void Mouse_Moved(GLFWwindow * window, double x, double y)
	{
		Mouse_Position_Screen.x = x;
		Mouse_Position_Screen.y = y;
		Mouse_Position.x = (x - (Screen_Width - Screen_Height) / 2) * 2.0 / Screen_Height - 1;
		Mouse_Position.y = -1 * (y * 2.0 / Screen_Height - 1);
		if (Relative_Mouse_Moved_Function)
		{
			static int last_x = 0;
			static int last_y = 0;
			Relative_Mouse_Moved_Function(x - last_x, y - last_y);
			last_x = x;
			last_y = y;
		}
		if (Mouse_Moved_Function)
		{
			Mouse_Moved_Function(x, y);
		}
		return;
	}

	// Default FPS-style mouse code, used intrinsically. Call FPS_Mouse to set up.
	void FPS_Mouse_Callback(int x, int y, Object * player, float sensitivity)
	{
		// Store the data from this call if it's required so that the callback works
		static float stored_sensitivity;
		static Object * stored_player;
		if (player)
			stored_player = player;
		if (sensitivity)
			stored_sensitivity = sensitivity;
		if (!sensitivity && stored_sensitivity)
			sensitivity = stored_sensitivity;
		if (!player && stored_player)
			player = stored_player;
		if ((!sensitivity && !stored_sensitivity) || (!player && !stored_player))
		{
			std::cout << "Error. FPS_Mouse_Callback is a function used intrinsically by Indigo Engine." << std::endl
				<< "Do not call unless you know what you are doing. Use FPS_Mouse(playerObject) to setup." << std::endl
				<< "Resetting mouse callback to nullptr to prevent further error. Passing as warning." << std::endl;
			Relative_Mouse_Moved_Function = nullptr;
			return;
		}
		float y_angle = player->Facing.Get_Y_Angle() + y * -1 * sensitivity;
		if (!(y_angle > 89 && y_angle < 271))
		{
			player->Facing.Add_Direction(0.0, x * sensitivity, y * -1 * sensitivity);
		}
		else
		{
			if (y_angle < 180)
			{
				player->Facing.Set_Direction(1.0, player->Facing.Get_X_Angle() + x * sensitivity, 89);
			}
			else
			{
				player->Facing.Set_Direction(1.0, player->Facing.Get_X_Angle() + x * sensitivity, 271);
			}
		}
	}

	// Default parameter call, needed for FPS mouse callback. Used intrinsically, do not call.
	void FPS_Mouse_Function(int x, int y)
	{
		FPS_Mouse_Callback(x, y);
	}

	// Default FPS-style mouse for looking around. Set an object pointer that sets onto your camera.
	// Then, use Indigo::Current_World.camera.facing = player.facing;
	void FPS_Mouse(Object * player, float sensitivity)
	{
		static bool enable = true;
		if (enable)
		{
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			FPS_Mouse_Callback(0, 0, player, sensitivity);
			Relative_Mouse_Moved_Function = FPS_Mouse_Function;
		}
		else
		{
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			Relative_Mouse_Moved_Function = nullptr;
		}
		enable = !enable;
		return;
	}

	// Acts for when an error is encountered
	void Error_Found(int type, const char * message)
	{
		std::cout << "Error. " << message << std::endl;
		if (Error_Function)
		{
			Error_Function(type, message);
		}
	}

	// Updates world
	void Update(const float time)
	{
		Actual_FPS = 1000 / time;
		if (Update_Function)
		{
			Update_Function(time);
		}
		Current_World.Update(time);
		return;
	}

	// Renders world
	void Render(void)
	{
		if (Render_Function)
		{
			Render_Function();
		}
		Current_World.Render();
		glfwSwapBuffers(Indigo::Window);
		glfwPollEvents();
		return;
	}

	// Callback for the spashscreen
	void Animate_Splash(float time, Object& self)
	{
		static float total = 0;
		static int last_frame = -1;
		total += time;
		int frame = int(total / 32);
		if (self.Color.a == 0)
		{
			if (total > 500)
			{
				total = 0;
				self.Color.a = 1;
			}
		}
		else if (frame < 61)
		{
			if (last_frame != frame)
			{
				glDeleteTextures(1, &self.Data.Texture_ID);
				self.Data.Texture_ID = self.User_Data[frame];
				last_frame = frame;
			}
		}
		else
		{
			if (frame > 100)
			{
				self.Color.a -= time / 1500;
				if (self.Color.a <= 0)
				{
					total = 0;
					last_frame = 0;
					Indigo::Current_World.Remove_2D_Object(self.ID);
				}
			}
		}
	}

	// Callback for the spashscreen's text
	void Splash_Label_Fade(float time, Object& self)
	{
		static float total = 0;
		static bool started = false;
		total += time;
		int frame = int(total / 32);
		if (!started)
		{
			if (total > 500)
			{
				total = 0;
				started = true;
			}
		}
		else if (frame > 50 && frame < 61)
		{
			self.Color.a = (frame - 50) / 10.0;
		}
		else if (frame > 100)
		{
			self.Color.a -= time / 1500;
			if (self.Color.a <= 0)
			{
				total = 0;
				started = false;
				Indigo::Current_World.Remove_2D_Object(self.ID);
			}
		}
	}

	// A nice little splashscreen for load routines
	void Construct_Splash(World& add_to)
	{

		Object splash_screen = Object(0, 0, 0, Mesh(), Animate_Splash);
		Mesh splash;
		std::vector<glm::vec3> splashverts;
		std::vector<glm::vec2> splashuvs;
		std::vector<glm::vec3> splashnormals;
		for (int i = 0; i < 61; ++i)
		{
			splashverts.push_back(glm::vec3(-0.75, -0.5, 0));
			splashverts.push_back(glm::vec3(0.75, 1, 0));
			splashverts.push_back(glm::vec3(-0.75, 1, 0));
			splashverts.push_back(glm::vec3(-0.75, -0.5, 0));
			splashverts.push_back(glm::vec3(0.75, -0.5, 0));
			splashverts.push_back(glm::vec3(0.75, 1, 0));
		}
		for (int i = 0; i < 61; ++i)
		{
			splashuvs.push_back(glm::vec2(0, 1));
			splashuvs.push_back(glm::vec2(1, 0));
			splashuvs.push_back(glm::vec2(0, 0));
			splashuvs.push_back(glm::vec2(0, 1));
			splashuvs.push_back(glm::vec2(1, 1));
			splashuvs.push_back(glm::vec2(1, 0));
		}
		for (int i = 0; i < 61 * 6; ++i)
		{
			splashnormals.push_back(glm::vec3(0, -1, 0));
		}
		//splash.Initialize(splashverts, splashuvs, splashnormals);
		for (int i = 0; i < 61; ++i)
		{
			std::string frame = ("Textures/SplashScreen/" + std::to_string(i) + ".png");
			splash.Texture_ID = 0;
			splash.Texture(frame.c_str());
			splash_screen.User_Data.push_back(splash.Texture_ID);
		}
		splash_screen.Color = glm::vec4(1, 1, 1, 0);
		splash_screen.Data = splash;
		splash_screen.Length_Index = 6;
		add_to.Add_2D_Object(splash_screen);
		std::vector<glm::vec3> textverts;
		std::vector<glm::vec2> textuvs;
		std::vector<glm::vec3> textnormals;
		textverts.push_back(glm::vec3(-1, -1, 0));
		textverts.push_back(glm::vec3(1, 1, 0));
		textverts.push_back(glm::vec3(-1, 1, 0));
		textverts.push_back(glm::vec3(-1, -1, 0));
		textverts.push_back(glm::vec3(1, -1, 0));
		textverts.push_back(glm::vec3(1, 1, 0));
		textuvs.push_back(glm::vec2(0, 1));
		textuvs.push_back(glm::vec2(1, 0));
		textuvs.push_back(glm::vec2(0, 0));
		textuvs.push_back(glm::vec2(0, 1));
		textuvs.push_back(glm::vec2(1, 1));
		textuvs.push_back(glm::vec2(1, 0));
		for (int i = 0; i < 6; ++i)
		{
			textnormals.push_back(glm::vec3(0, -1, 0));
		}
		Mesh label;
		//label.Initialize(textverts, textuvs, textnormals);
		label.Texture("Textures/SplashScreen/Label.png");
		add_to.Add_2D_Object(Object(0, 0, 0, label, Splash_Label_Fade, 0, Direction(), glm::vec4(1, 1, 1, 0)));
		return;

	}

	// Get elapsed time in the game, optional minus for partial times, in milliseconds
	inline float Elapsed()
	{
		return glfwGetTime() * 1000;
	}

	// Get the floating point equivalent and the length of string with standard notation assuming start of float is at start
	float Fast_Float(const char * stream, int* output, const int start)
	{
		int i = start;
		for (; stream[i] == ' '; ++i);
		int negative = 1;
		if (stream[i] == '-')
		{
			negative = -1;
			++i;
		}
		float result = 0;
		for (; stream[i] >= '0' && stream[i] <= '9'; ++i)
		{
			result = result * 10 + (stream[i] - '0');
		}
		if (stream[i] != '.')
		{
			result *= negative;
			if (output)
			{
				*output = i;
			}
			return result;
		}
		++i;
		float precision = 0.1;
		for (; stream[i] >= '0' && stream[i] <= '9'; ++i)
		{
			result += precision * (stream[i] - '0');
			precision *= 0.1;
		}
		result *= negative;
		if (output)
		{
			*output = i;
		}
		return result;
	}


	// Get the letter as if the key was pressed with shift. Useful for Key Callbacks / checking
	char Get_Shifted_Character(const char key)
	{
		if (key == '`')
			return '~';
		if (key == '1')
			return '!';
		if (key == '2')
			return '@';
		if (key == '3')
			return '#';
		if (key == '4')
			return '$';
		if (key == '5')
			return '%';
		if (key == '6')
			return '^';
		if (key == '7')
			return '&';
		if (key == '8')
			return '^';
		if (key == '9')
			return '(';
		if (key == '0')
			return ')';
		if (key == '-')
			return '_';
		if (key == '=')
			return '+';
		if (key >= 'a' && key <= 'z')
			return key + 'A' - 'a';
		if (key == '[')
			return '{';
		if (key == ']')
			return '}';
		if (key == '\\')
			return '|';
		if (key == ';')
			return ':';
		if (key == '\'')
			return '"';
		if (key == ',')
			return '<';
		if (key == '.')
			return '>';
		if (key == '/')
			return '?';
		return key;
	}


	// Get all the errors since last error dump to main console window
	void Error_Dump(void)
	{
		std::cout << "------ Begin Error Dump --------" << std::endl;
		int code = -1;
		while (code != GL_NO_ERROR)
		{
			code = glGetError();
			if (code != GL_NO_ERROR)
			{
				std::cout << code << std::endl;
			}
		}
		std::cout << "------- End Error Dump ---------" << std::endl;
	}


	// Stores the current world to render
	World Current_World;

	// Stores the window we're rendering onto
	GLFWwindow * Window = nullptr;


	// Stores the function to call when a key is pressed
	void(*Key_Pressed_Function)(int key);

	// ... when a key is released
	void(*Key_Released_Function)(int key);

	// ... when the mouse is pressed or released. Given in 2D_Object space
	void(*Mouse_Button_Function)(int button, int state, float x, float y);

	// ... when the mouse is moved
	void(*Mouse_Moved_Function)(int x, int y);

	// ... when the mouse is moved, given relative to the center.
	// Also hides mouse when defined.
	void(*Relative_Mouse_Moved_Function)(int x, int y);
	
	// ... every time the world updates
	void(*Update_Function)(float time);

	// ... just before the rendering of objects in the world
	void(*Render_Function)(void);

	// ... on an error
	void(*Error_Function)(int type, const char * message);


	// Members with the index of a key which is currently down are true, always lowercase. Abnormals are GLFW_ESCAPE, etc.
	bool Keys[512];

	// Don't use, used internally. Use Pressed('a') instead.
	bool Keys_Pressed[512];

	// Stores whether modifiers are pressed
	bool Shift;

	bool Control;

	bool Alt;

	// Stores whether mouse buttons are down
	bool Left_Mouse = false;

	bool Right_Mouse = false;

	bool Middle_Mouse = false;

	glm::vec2 Mouse_Position = glm::vec2(0, 0);

	glm::vec2 Mouse_Position_Screen = glm::vec2(0, 0);


	// The index for the VAO thingy, not probably important.
	unsigned int VAO;


	// Stores the width of the screen
	int Screen_Width = 0;

	// Stores the height of the screen
	int Screen_Height = 0;

	// Stores the aspect ratio of the screen
	float Aspect_Ratio = 1;

	// Stores the milliseconds to add between each frame
	float Frame_Length_Minimum = 0;

	// Stores the current actual FPS of the update loop
	int Actual_FPS = 60;


	// Colors

	glm::vec3 White_Color(1.0, 1.0, 1.0);

	glm::vec3 Black_Color(0.0, 0.0, 0.0);

	glm::vec3 Sky_Color(0.5, 0.8, 1.0);

	glm::vec3 Red_Color(0.4, 0.0, 0.0);

	glm::vec3 Green_Color(0.0, 0.8, 0.0);

	glm::vec3 Blue_Color(0.0, 0.0, 0.6);

	glm::vec3 Light_Blue_Color(0.4, 0.7, 0.9);

	glm::vec3 Orange_Color(1.0, 0.4, 0.0);

}