// Basic OpenGL functions, also takes care of most of the background work.
// These function will mostly be called in Main.

#pragma once

#include "World.h"
//#include "Animation.h"


namespace Indigo
{
	// Initializes window and rendering matrices.
	void Initialize(int argc = 0, char ** argv = nullptr, const char * window_name = "Indigo",
		const int& max_framerate = 48, const bool& fullscreen = 1, float * background = nullptr,
		const int& window_width = 800, const int& window_height = 600,
		const int& field_of_view = 60);
	// Starts the main loop with update, render, and input
	void Run(void);
	// Acts for when the window reshapes
	void Reshape(int width = 0, int height = 0);
	// Acts for keys which act once, and stores for multi-acting keys
	void Key_Pressed(unsigned char key, int x, int y);
	// Acts for keys which act on release, and removes stored keys
	void Key_Released(unsigned char key, int x, int y);
	// Acts for when the mouse is pressed or released
	void Mouse_Button(int button, int state, int x, int y);
	// Acts for when the mouse is moved
	void Mouse_Moved(int x, int y);
	// Updates world
	void Update(int trash);
	// Renders world
	void Render(void);

	// Get elapsed time in the game, optional modulo for partial times
	int Elapsed(const int& modulo = 0);

	// Stores the current world to render
	extern World Current_World;
	// Stores the milliseconds to add between each frame
	extern int Frame_Length_Minimum;
	// Stores the field of view
	extern int Field_Of_View;
	// Stores the function to call when a key is pressed
	extern void(*Key_Pressed_Function)(unsigned char key, int x, int y);
	// ... when a key is released
	extern void(*Key_Released_Function)(unsigned char key, int x, int y);
	// ... when the mouse is pressed or released
	extern void(*Mouse_Button_Function)(int button, int state, int x, int y);
	// ... when the mouse is moved
	extern void(*Mouse_Moved_Function)(int x, int y);
	// ... when the mouse is moved, given relative to the center
	// Also hides mouse when defined.
	extern void(*Relative_Mouse_Moved_Function)(int x, int y);
	// ... every time the world updates
	extern void(*Update_Function)(int time);
	// ... just before the rendering of objects in the world
	extern void(*Render_Function)(void);
	// Members with the index of a key which is currently down are true, always lowercase
	extern bool keys[256];
	// Stores whether shift is pressed
	extern bool Shift;

	// Colors
	extern float White_Color[3];
	extern float Black_Color[3];
	extern float   Sky_Color[3];
	extern float   Red_Color[3];
	extern float Green_Color[3];
	extern float  Blue_Color[3];

}