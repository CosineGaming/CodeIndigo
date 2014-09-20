// Basic OpenGL functions, also takes care of most of the background work.
// These function will mostly be called in Main.

#pragma once

#include "World.h"
#include "Animation.h"

#include <thread>
#include <iostream>
#include <vector>
#include "GLFW/glfw3.h"

#define Hash_Modulo 4294967291 // Largest Prime < 2^32 used for 32-bit integer simple fast hashes


namespace Indigo
{
	// Initializes window and rendering matrices. Really ought to be called first.
	void Initialize(const char * window_name = "Indigo", glm::vec3 background = glm::vec3(0.5, 0.8, 1.0), const int aa_samples = 4,
		const int max_framerate = 60, const bool create_world = true, const int window_width = 0, const int window_height = 0, const bool fullscreen = true);

	// Starts the main loop with update, render, and input. Returns 0 ideally... 1 on problem.
	int Run(void);
	// Exits the loop, destroying the window. More contexts can still be created.
	void Close(void);

	// Acts for when the window reshapes
	void Reshape(GLFWwindow * window, int width = 0, int height = 0);
	// Acts for keys which act once, and stores for multi-acting keys
	void Key_Action(GLFWwindow * window, int key, int scancode, int action, int modifiers);
	// Checks whether a key has just been pressed
	bool Pressed(unsigned char key);
	// Acts for when the mouse is pressed or released
	void Mouse_Button(GLFWwindow * window, int button, int state, int modifiers);
	// Acts for when the mouse is moved
	void Mouse_Moved(GLFWwindow * window, double x, double y);
	// Default FPS-style mouse code, used intrinsically. Call FPS_Mouse to set up.
	void FPS_Mouse_Callback(int x, int y, Object * player = nullptr, float sensitivity = 0);
	// Default parameter call, needed for FPS mouse callback. Used intrinsically, do not call.
	void FPS_Mouse_Function(int x, int y);
	// Default FPS-style mouse for looking around. Set an object pointer that sets onto your camera.
	// Then, use Indigo::Worlds[0].camera.facing = player.facing;
	void FPS_Mouse(Object * player = nullptr, float sensitivity = 0.2);
	// Acts for when an error is encountered
	void Error_Found(int type, const char * message);
	// Updates world
	void Update(const float time);
	// Renders world
	void Render(void);

	// Set a rectangle's update function to this for a neat Indigo Engine splashscreen
	void Splash_Screen(float time, Object& self);

	// Get elapsed time in the game, optional modulo for partial times, in seconds
	inline float Elapsed();
	// Get the floating point equivalent and the length of string with standard notation assuming start of float is at start
	float Fast_Float(const char * stream, int* output = nullptr, const int start = 0);
	// Get the letter as if the key was pressed with shift. Useful for Key Callbacks / checking
	char Get_Shifted_Character(const char key);
	// Get all the errors since last error dump to main console window
	void Error_Dump(void);

	// Point to the (Active_Worlds) worlds you would like rendered each frame
	extern World * Worlds;
	// The number of worlds pointed with Worlds and beyond (2 -> Worlds[0], Worlds[1]) to render each frame
	extern unsigned int Active_Worlds;
	// Stores the window we're rendering onto
	extern GLFWwindow * Window;

	// Stores the function to call when a key is pressed
	extern void(*Key_Pressed_Function)(int key);
	// ... when a key is released
	extern void(*Key_Released_Function)(int key);
	// ... when the OS identifies a held key and repeats
	extern void(*Key_Repeat_Function)(int key);
	// ... when the mouse is pressed or released. Given in 2D_Object space
	extern void(*Mouse_Button_Function)(int button, int state, float x, float y);
	// ... when the mouse is moved
	extern void(*Mouse_Moved_Function)(int x, int y);
	// ... when the mouse is moved, given relative to the center
	// Also hides mouse when defined.
	extern void(*Relative_Mouse_Moved_Function)(int x, int y);
	// ... when the window is resized
	extern void(*Reshape_Function)(int width, int height);
	// ... on an error
	extern void(*Error_Function)(int type, const char * message);
	// ... every time the world updates
	extern void(*Update_Function)(float time);
	// ... just before the rendering of objects in the world
	extern void(*Render_Function)(void);

	// Members with the index of a key which is currently down are true, always lowercase. Abnormals are GLFW_ESCAPE, etc.
	extern bool Keys[512];
	// Don't use, used internally. Use Pressed('a') instead.
	extern bool Keys_Pressed[512];
	// Stores whether modifiers are pressed
	extern bool Shift;
	extern bool Control;
	extern bool Alt;
	// Stores whether mouse buttons are down and where the cursor is
	extern bool Left_Mouse;
	extern bool Right_Mouse;
	extern bool Middle_Mouse;
	extern glm::vec2 Mouse_Position;
	extern glm::vec2 Mouse_Position_Screen;

	// Stores the width of the screen
	extern int Screen_Width;
	// Stores the height of the screen
	extern int Screen_Height;
	// Stores the aspect ratio of the screen
	extern float Aspect_Ratio;
	// Stores the seconds to add between each frame
	extern float Frame_Length_Minimum;
	// Enable motion blur at a cost to speed by setting Motion_Blur to a higher number (default 1: no blur)
	extern int Motion_Blur;
	// Stores the current actual FPS of the update loop
	extern int Actual_FPS;

	// Colors
	extern glm::vec3 White_Color;
	extern glm::vec3 Black_Color;
	extern glm::vec3 Sky_Color;
	extern glm::vec3 Red_Color;
	extern glm::vec3 Green_Color;
	extern glm::vec3 Blue_Color;
	extern glm::vec3 Light_Blue_Color;
	extern glm::vec3 Orange_Color;

}