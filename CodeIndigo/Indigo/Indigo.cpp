// Basic OpenGL functions, also takes care of most of the background work.
// These function will mostly be called in Main.

#include "Indigo.h"
#include <stdlib.h>
#include "glut.h"
#include <iostream>


namespace Indigo
{
	// Initializes window and rendering matrices.
	void Initialize(int argc, char ** argv, const char * window_name, const int max_framerate, const bool fullscreen, float * background,
		const int window_width, const int window_height, const int field_of_view)
	{
		// Initiate glut
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
		glutInitWindowSize(window_width, window_height);
		glutCreateWindow(window_name);
		if (fullscreen)
		{
			glutFullScreen();
		}
		if (!background)
		{
			background = Sky_Color;
		}
		glClearColor(background[0], background[1], background[2], 1.0);

		// Set callbacks
		Frame_Length_Minimum = 1000 / max_framerate;
		Field_Of_View = field_of_view;
		glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
		glutDisplayFunc(Render);
		glutTimerFunc(10, Update, 0);
		glutReshapeFunc(Reshape);
		glutPassiveMotionFunc(Mouse_Moved);
		glutMotionFunc(Mouse_Moved);
		glutMouseFunc(Mouse_Button);
		glutKeyboardFunc(Key_Pressed);
		glutKeyboardUpFunc(Key_Released);

		// Setup fog
		glEnable(GL_FOG);
		glFogfv(GL_FOG_COLOR, White_Color);
		glHint(GL_FOG_HINT, GL_DONT_CARE);
		glFogf(GL_FOG_DENSITY, 0.002);

		// Enable rendering options
		glShadeModel(GL_SMOOTH);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		return;
	}

	// Starts the main loop with update, render, and input
	void Run(void)
	{
		glutMainLoop();
		return;
	}

	// Acts for when the window reshapes
	void Reshape(int width, int height)
	{
		if (0 == width || 0 == height)
		{
			width = glutGet(GLUT_WINDOW_WIDTH);
			height = glutGet(GLUT_WINDOW_HEIGHT);
		}
		else
		{
			glViewport(0, 0, width, height);
		}
		Screen_Width = width;
		Screen_Height = height;
		Aspect_Ratio = (float) width / (float) height;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(Field_Of_View, Aspect_Ratio, 0.01, 500.0);
		glFogf(GL_FOG_START, 400.0);
		glFogf(GL_FOG_END, 500.0);
		glMatrixMode(GL_MODELVIEW);
	}

	// Acts for when the window reshapes in an Orthographic environment
	void Reshape_2D(int width, int height)
	{
		if (0 == width || 0 == height)
		{
			width = glutGet(GLUT_WINDOW_WIDTH);
			height = glutGet(GLUT_WINDOW_HEIGHT);
		}
		else
		{
			glViewport(0, 0, width, height);
		}
		Aspect_Ratio = (float) width / (float) height;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-1 * Aspect_Ratio, Aspect_Ratio, -1, 1, -1, 1);
		glMatrixMode(GL_MODELVIEW);
	}

	// Acts for Keys which act once, and stores for multi-acting Keys
	void Key_Pressed(unsigned char key, int x, int y)
	{
		// Convert uppercases to lowercase
		if (key >= 65 && key <= 90)
		{
			key += 32;
		}
		if (Key_Pressed_Function)
		{
			Key_Pressed_Function(key, x, y);
		}
		Keys[key] = true;
		Keys_Pressed[key] = true;
		Shift = glutGetModifiers() == GLUT_ACTIVE_SHIFT;
		return;
	}

	// Acts for Keys which act on release, and removes stored Keys
	void Key_Released(unsigned char key, int x, int y)
	{
		Shift = glutGetModifiers() == GLUT_ACTIVE_SHIFT;
		// Convert uppercases to lowercase
		if (key >= 65 && key <= 90)
		{
			key += 32;
		}
		if (Key_Released_Function)
		{
			Key_Released_Function(key, x, y);
		}
		Keys[key] = false;
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
	void Mouse_Button(int button, int state, int x, int y)
	{
		Shift = glutGetModifiers() == GLUT_ACTIVE_SHIFT;
		if (button == GLUT_LEFT_BUTTON)
		{
			Left_Mouse = state == GLUT_DOWN;
		}
		if (button == GLUT_RIGHT_BUTTON)
		{
			Right_Mouse = state == GLUT_DOWN;
		}
		if (button == GLUT_MIDDLE_BUTTON)
		{
			Middle_Mouse = state == GLUT_DOWN;
		}
		if (Mouse_Button_Function)
		{
			Mouse_Button_Function(button, state, (x - (glutGet(GLUT_WINDOW_WIDTH) - glutGet(GLUT_WINDOW_HEIGHT)) / 2) * 2.0 / glutGet(GLUT_WINDOW_HEIGHT) - 1,
				-1 * (y * 2.0 / glutGet(GLUT_WINDOW_HEIGHT) - 1));
		}
		if (Mouse_Raw_Button_Function)
		{
			Mouse_Raw_Button_Function(button, state, x, y);
		}
		return;
	}

	// Acts for when the mouse is moved
	void Mouse_Moved(int x, int y)
	{
		if (Relative_Mouse_Moved_Function)
		{
			int width = glutGet(GLUT_WINDOW_WIDTH);
			int height = glutGet(GLUT_WINDOW_HEIGHT);
			static int last_x = 0;
			static int last_y = 0;
			if (!((width / 2 == x && height / 2 == y)
				|| (0 == x - last_x && 0 == y - last_y)))
			{
				Relative_Mouse_Moved_Function(x - last_x, y - last_y);
			}
			static const int margin = 100;
			if (x < margin || x > width - margin || y < margin || y > height - margin)
			{
				glutWarpPointer(width / 2, height / 2);
			}
			last_x = x;
			last_y = y;
			glutSetCursor(GLUT_CURSOR_NONE);
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
				<< " Do not call unless you know what you are doing. Use FPS_Mouse(playerObject) to setup." << std::endl
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
	void FPS_Mouse(bool enable, Object * player, float sensitivity)
	{
		if (enable)
		{
			FPS_Mouse_Callback(0, 0, player, sensitivity);
			Relative_Mouse_Moved_Function = FPS_Mouse_Function;
		}
		else
		{
			glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
			Relative_Mouse_Moved_Function = nullptr;
		}
	}

	// Updates world
	void Update(int trash)
	{
		glutTimerFunc(Frame_Length_Minimum, Update, 0);
		static int last_time = 0;
		int delta_time = glutGet(GLUT_ELAPSED_TIME) - last_time;
		last_time = glutGet(GLUT_ELAPSED_TIME);
		Actual_FPS = 1000.0 / last_time;
		if (Update_Function)
		{
			Update_Function(delta_time);
		}
		Current_World.Update(delta_time);
		glutPostRedisplay();
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
		return;
	}

	// Get elapsed time in the game, optional minus for partial times
	int Elapsed(const int minus)
	{
		return glutGet(GLUT_ELAPSED_TIME) - minus;
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
		++i;
		float precision = 0.1;
		for (; stream[i] >= '0' && stream[i] <= '9'; ++i)
		{
			result += precision * (stream[i] - '0');
			precision *= 0.1;
		}
		++i;
		result *= negative;
		if (output)
		{
			*output = i;
		}
		return result;
	}


	// Stores the current world to render
	World Current_World;

	// Stores the milliseconds to add between each frame
	int Frame_Length_Minimum;

	// Stors the field of view
	int Field_Of_View;


	// Stores the function to call when a key is pressed
	void(*Key_Pressed_Function)(unsigned char key, int x, int y);

	// ... when a key is released
	void(*Key_Released_Function)(unsigned char key, int x, int y);

	// ... when the mouse is pressed or released. Given in 2D_Object space
	void(*Mouse_Button_Function)(int button, int state, float x, float y);

	// ... when the mouse is pressed or released. Given in Window Coordinates
	void(*Mouse_Raw_Button_Function)(int button, int state, float x, float y);

	// ... when the mouse is moved
	void(*Mouse_Moved_Function)(int x, int y);

	// ... when the mouse is moved, given relative to the center.
	// Also hides mouse when defined.
	void(*Relative_Mouse_Moved_Function)(int x, int y);

	// ... every time the world updates
	void(*Update_Function)(int time);

	// ... just before the rendering of objects in the world
	void(*Render_Function)(void);


	// Members with the index of a key which is currently down are true
	bool Keys[256];

	// Don't use, used internally. Use Pressed('a') instead.
	bool Keys_Pressed[256];

	// Stores whether shift is pressed
	bool Shift = false;

	// Stores whether mouse buttons are down
	bool Left_Mouse;

	bool Right_Mouse;

	bool Middle_Mouse;

	// Stores the current actual FPS of the update loop
	int Actual_FPS;


	// Stores the width of the screen
	int Screen_Width;

	// Stores the height of the screen
	int Screen_Height;

	// Stores the aspect ratio of the screen
	float Aspect_Ratio;


	// Colors

	float White_Color[3] = { 1.0, 1.0, 1.0 };

	float Black_Color[3] = { 0.0, 0.0, 0.0 };

	float Sky_Color[3] = { 0.5, 0.8, 1.0 };

	float Red_Color[3] = { 0.4, 0.0, 0.0 };

	float Green_Color[3] = { 0.0, 0.8, 0.0 };

	float Blue_Color[3] = { 0.0, 0.0, 0.6 };

	float Light_Blue_Color[3] = { 0.4, 0.7, 0.9 };

	float Orange_Color[3] = { 1.0, 0.4, 0.0 };

}