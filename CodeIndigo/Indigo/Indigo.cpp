// Basic OpenGL functions, also takes care of most of the background work.
// These function will mostly be called in Main.

#include "Indigo.h"
#include <stdlib.h>
#include "glut.h"


namespace Indigo
{
	// Initializes window and rendering matrices.
	void Initialize (int argc, char ** argv, const char * window_name,
		const int& window_width, const int& window_height, const bool& fullscreen,
		int field_of_view, float * background, int max_framerate)
	{
		glutInit (&argc, argv);
		glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
		glutInitWindowSize (window_width, window_height);
		glutCreateWindow (window_name);
		if (fullscreen)
		{
			glutFullScreen ();
		}
		if (background)
		{
			glClearColor (background [0], background [1], background [2], 1.0);
		}
		else
		{
			glClearColor (Sky_Color [0], Sky_Color [1], Sky_Color [2], 1.0);
		}
		Frame_Length_Minimum = 1000 / max_framerate;
		glutSetKeyRepeat (GLUT_KEY_REPEAT_OFF);
		glutDisplayFunc (Render);
		glutTimerFunc (10, Update, 0);
		glutReshapeFunc (Reshape);
		glutPassiveMotionFunc (Mouse_Moved);
		glutMouseFunc (Mouse_Button);
		glutKeyboardFunc (Key_Pressed);
		glutKeyboardUpFunc (Key_Released);
		glMatrixMode (GL_PROJECTION);
		Reshape ();
		glMatrixMode (GL_MODELVIEW);
		glLoadIdentity ();
		glShadeModel (GL_SMOOTH);
		glEnable (GL_DEPTH_TEST);
		glEnable (GL_AUTO_NORMAL);
		glEnable (GL_LIGHTING);
		return;
	}

	// Starts the main loop with update, render, and input
	void Run (void)
	{
		glutMainLoop ();
		return;
	}

	// Acts for when the window reshapes
	void Reshape (int width, int height)
	{
		bool viewport = true;
		if (0 == width)
		{
			width = glutGet (GLUT_WINDOW_WIDTH);
			viewport = false;
		}
		if (0 == height)
		{
			height = glutGet (GLUT_WINDOW_HEIGHT);
			viewport = false;
		}
		if (viewport)
		{
			glViewport (0, 0, width, height);
		}
		glLoadIdentity ();
		gluPerspective (Field_Of_View,
			(float) width / (float) height,
			0.01, 30.0);
	}

	// Acts for keys which act once, and stores for multi-acting keys
	void Key_Pressed (unsigned char key, int x, int y)
	{
		if (Key_Pressed_Function)
		{
			Key_Pressed_Function (key, x, y);
		}
		keys [key] = true;
		return;
	}

	// Acts for keys which act on release, and removes stored keys
	void Key_Released (unsigned char key, int x, int y)
	{
		if (Key_Released_Function)
		{
			Key_Released_Function (key, x, y);
		}
		keys [key] = false;
		return;
	}

	// Acts for when the mouse is pressed or released
	void Mouse_Button (int button, int state, int x, int y)
	{
		if (Mouse_Button_Function)
		{
			Mouse_Button_Function (button, state, x, y);
		}
		return;
	}

	// Acts for when the mouse is moved
	void Mouse_Moved (int x, int y)
	{
		if (Relative_Mouse_Moved_Function)
		{
			int width = glutGet (GLUT_WINDOW_WIDTH);
			int height = glutGet (GLUT_WINDOW_HEIGHT);
			static int last_x = 0;
			static int last_y = 0;
			if (!((width / 2 == x && height / 2 == y)
				|| (0 == x - last_x && 0 == y - last_y)))
			{
				Relative_Mouse_Moved_Function (x - last_x, y - last_y);
			}
			static const int margin = 75;
			if (x < margin || x > width - margin || y < margin || y > height - margin)
			{
				glutWarpPointer (width / 2, height / 2);
			}
			last_x = x;
			last_y = y;
			glutSetCursor (GLUT_CURSOR_NONE);
		}
		if (Mouse_Moved_Function)
		{
			Mouse_Moved_Function (x, y);
		}
		return;
	}

	// Updates world
	void Update (int frame)
	{
		if (Update_Function)
		{
			Update_Function (frame);
		}
		Current_World.Update (frame);
		glutPostRedisplay ();
		glutTimerFunc (Frame_Length_Minimum, Update, frame + 1);
		return;
	}

	// Renders world
	void Render (void)
	{
		if (Render_Function)
		{
			Render_Function ();
		}
		Current_World.Render ();
		return;
	}



	// Stores the current world to render
	World Current_World;

	// Stores the milliseconds to add between each frame
	int Frame_Length_Minimum;

	// Stors the field of view
	int Field_Of_View;

	// Stores the function to call when a key is pressed
	void (*Key_Pressed_Function) (unsigned char key, int x, int y);

	// ... when a key is released
	void (*Key_Released_Function) (unsigned char key, int x, int y);

	// ... when the mouse is pressed or released
	void (*Mouse_Button_Function) (int button, int state, int x, int y);

	// ... when the mouse is moved
	void (*Mouse_Moved_Function) (int x, int y);

	// ... when the mouse is moved, given relative to the center.
		// Also hides mouse when defined.
	void (*Relative_Mouse_Moved_Function) (int x, int y);

	// ... every time the world updates
	void (*Update_Function) (int frame);

	// ... just before the rendering of objects in the world
	void (*Render_Function) (void);
	
	// Members with the index of a key which is currently down are true
	bool keys [256];


	// Colors
	
	float White_Color [3] = {1.0, 1.0, 1.0};

	float Black_Color [3] = {0.0, 0.0, 0.0};

	float   Sky_Color [3] = {0.5, 0.8, 1.0};

}