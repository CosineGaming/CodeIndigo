#include "Indigo\IndigoEngine.h"
#include <iostream>

Direction camera = Direction(6);

int start;

int placing = 2;
int value = -1;

void rotate(int x, int y)
{
	static int last_x = 0;
	static int last_y = 0;
	int right = x - last_x;
	int up = y - last_y;
	last_x = x;
	last_y = y;
	if (Indigo::Right_Mouse)
	{
		camera.Add_Direction(0, right * 0.1, up * 0.1);
		Indigo::Current_World.View.Place(camera.Get_X(), camera.Get_Y(), camera.Get_Z());
		Indigo::Current_World.View.Look_At(0, 0, 0);
	}
	else if (placing != 0)
	{
		static float partial = value;
		partial += 0.1 * (x > y ? x : y);
		value = int(partial);
		if (value > 1)
		{
			value = 1;
			partial = 1;
		}
		if (value < -1)
		{
			value = -1;
			partial = -1;
		}
	}
}

void place(int button, int state, float x, float y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		if (placing == 0)
		{
			glutSetCursor(GLUT_CURSOR_NONE);
			placing = 3;
		}
		else
		{
			static int x = 0;
			static int y = 0;
			static int z = 0;
			if (placing == 3)
			{
				x = value;
				placing = 2;
			}
			if (placing == 2)
			{
				y = value;
				placing = 1;
			}
			if (placing == 1)
			{
				z = value;
				Object& picked = Indigo::Current_World.Get_Object((z + 1) * 9 + (y + 1) * 3 + z + 1);
				if (picked.Line)
				{
					picked.Line = false;
				}
				else
				{
					placing = 3;
				}
				placing = 0;
			}
		}
		/*
		Indigo::Reshape();
		Indigo::Current_World.View.Look();
		double z;
		double X;
		double Y;
		double Z;
		double view[16];
		double projection[16];
		int window[4];
		glGetDoublev(GL_MODELVIEW_MATRIX, view);
		glGetDoublev(GL_PROJECTION_MATRIX, projection);
		glGetIntegerv(GL_VIEWPORT, window);
		glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
		std::cout << z << ": " << std::endl;
		if (gluUnProject(x, Indigo::Screen_Width - y, z, view, projection, window, &X, &Y, &Z))
		{
			Indigo::Current_World.Add_Object(Object(X, Y, Z, Mesh::Sphere(0.1)));
			std::cout << X << ", " << Y << ", " << Z << std::endl;
		}*/
	}
}

void load(int time)
{
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			for (int z = -1; z <= 1; ++z)
			{
				start = Indigo::Current_World.Number_Of_Objects();
				Indigo::Current_World.Add_Object(Object(x, y, z, Mesh::Cube(1), nullptr, nullptr, nullptr, true, Direction(1,0,0), false, true, 256, true));
			}
		}
	}
	Indigo::Current_World.Light_Setup.Add_Light(0.0, 5.0, 5.0);
	Indigo::Current_World.View.Place(camera.Get_X(), camera.Get_Y(), camera.Get_Z());
	Indigo::Current_World.View.Look_At(0, 0, 0);
	Indigo::Update_Function = nullptr;
}

int main(int argc, char ** argv)
{
	Indigo::Initialize(argc, argv, "Tic-Tac-Toe");
	Indigo::Mouse_Moved_Function = rotate;
	Indigo::Mouse_Raw_Button_Function = place;
	Indigo::Update_Function = load;
	Indigo::Run();
}