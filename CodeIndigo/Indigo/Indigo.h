// Basic OpenGL functions, also takes care of most of the background work.
// These function will mostly be called in Main.

#pragma once

#include "World.h"
#include "Animation.h"


namespace Indigo
{
	// Initializes window and rendering matrices.
	void Initialize(int argc, char ** argv, const char * window_name = "Indigo", const int max_framerate = 120, const bool fullscreen = true, float * background = nullptr,
		const int window_width = 800, const int window_height = 600);

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
	int Elapsed(const int minus = 0);
	// Get the floating point equivalent and the length of string with standard notation assuming start of float is at start
	float Fast_Float(const char * stream, int* output = nullptr, const int start = 0);

	// Stores the current world to render
	extern World Current_World;
	// Stores the milliseconds to add between each frame
	extern int Frame_Length_Minimum;

	// Stores the function to call when a key is pressed
	extern void(*Key_Pressed_Function)(unsigned char key, int x, int y);
	// ... when a key is released
	extern void(*Key_Released_Function)(unsigned char key, int x, int y);
	// Checks whether a key has just been pressed
	extern bool Pressed(unsigned char key);
	// ... when the mouse is pressed or released. Given in 2D_Object space
	extern void(*Mouse_Button_Function)(int button, int state, float x, float y);
	// ... when the mouse is pressed or released. Given in Window Coordinates
	extern void(*Mouse_Raw_Button_Function)(int button, int state, float x, float y);
	// ... when the mouse is moved
	extern void(*Mouse_Moved_Function)(int x, int y);
	// ... when the mouse is moved, given relative to the center
	// Also hides mouse when defined.
	extern void(*Relative_Mouse_Moved_Function)(int x, int y);
	// Default FPS-style mouse code, used intrinsically. Call FPS_Mouse to set up.
	void FPS_Mouse_Callback(int x, int y, Object * player = nullptr, float sensitivity = 0);
	// Default parameter call, needed for FPS mouse callback. Used intrinsically, do not call.
	void FPS_Mouse_Function(int x, int y);
	// Default FPS-style mouse for looking around. Set an object pointer that sets onto your camera.
	// Then, use Indigo::Current_World.camera.facing = player.facing;
	void FPS_Mouse(bool enable, Object * player = nullptr, float sensitivity = 0.2);
	// Use the default FPS-style mouse by calling this, then all 
	// ... every time the world updates
	extern void(*Update_Function)(int time);
	// ... just before the rendering of objects in the world
	extern void(*Render_Function)(void);

	// Members with the index of a key which is currently down are true, always lowercase
	extern bool Keys[256];
	// Don't use, used internally. Use Pressed('a') instead.
	extern bool Keys_Pressed[256];
	// Stores whether shift is pressed
	extern bool Shift;
	// Stores whether mouse buttons are down
	extern bool Left_Mouse;
	extern bool Right_Mouse;
	extern bool Middle_Mouse;
	// Stores the current actual FPS of the update loop
	extern int Actual_FPS;

	// Stores the width of the screen
	extern int Screen_Width;
	// Stores the height of the screen
	extern int Screen_Height;
	// Stores the aspect ratio of the screen
	extern float Aspect_Ratio;

	// Colors
	extern float White_Color[3];
	extern float Black_Color[3];
	extern float Sky_Color[3];
	extern float Red_Color[3];
	extern float Green_Color[3];
	extern float Blue_Color[3];
	extern float Light_Blue_Color[3];
	extern float Orange_Color[3];

}