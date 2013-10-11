// Graphical objects to be displayed on the screen,
// along with useful functions for drawing and warping them

#include "Object.h"
#include "Indigo.h"
#include <stdlib.h>
#include <iostream> // DELETE
#include "glut.h"


Object::Object (const float& x, const float& y, const float& z,
	const Mesh& mesh, float *color, float shine,
	void (*update_function) (const int& frame, Object& self),
	const bool& line)
{
	Place (x, y, z);
	Data = mesh;
	object_color = color;
	object_shine = shine;
	Update = update_function;
	Line = line;
	Is_Blank = mesh.Size () == 0;
	ID = -1;
	return;
}


// Copy an object, used as main constructor
Object::Object (const Object& object)
{
	Place (object.X, object.Y, object.Z);
	Data = object.Data;
	object_color = object.object_color;
	object_shine = object.object_shine;
	Update = object.Update;
	Line = object.Line;
	Is_Blank = object.Is_Blank;
	ID = object.ID;
	return;
}


// Destroys the object
Object::~Object (void)
{
	return;
}


// Renders the object
void Object::Render (void) const
{
	static int skip; // DELETE
	glPushMatrix ();
	float full_array [] = {1.0, 1.0, 1.0, 1.0};
	glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, object_color ? object_color : full_array);
	glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, full_array);
	glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, object_shine);
	glTranslatef (X, Y, Z);
	if (Line)
	{
		glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	}
	std::vector <Vertex> points = Data.Get_Vertices ();
	glBegin (Render_Types [Data.Group_Size]);
	int flipped = 0; // DELETE
	for (int Point=0; Point<points.size (); Point++)
	{
		// When each polygon is finished, calculate a light normal
		if (Point % (Data.Group_Size != 0 ? Data.Group_Size : points.size ()) == 0 ||
			(Data.Group_Size == 0 && Point <= points.size () - 3))
		{
			Vertex two = points [Point + 1] - points [Point];
			Vertex three = points [Point + 2] - points [Point];
			points [Point].To_Direction ();
			Direction normal = Direction::Coordinates (
				two.Y * three.Z - two.Z * three.Y,
				-1 * (two.Z * three.X - two.X * three.Z),
				two.X * three.Y - two.Y * three.X);
			normal.Normalize ();
			Direction pointing = Direction::Coordinates (
				Indigo::Current_World.camera.X,
				Indigo::Current_World.camera.Y,
				Indigo::Current_World.camera.Z)
				.Distance (points [Point].To_Direction ());
			glEnd ();
			glBegin (GL_LINES);
			normal.Add_Direction (-0.95);
			Vertex Cursor = points [Point];
			glVertex3f (Cursor.X, Cursor.Y, Cursor.Z);
			glVertex3f (Cursor.X + normal.Get_X (),
				Cursor.Y + normal.Get_Y (), Cursor.Z + normal.Get_Z ());
			normal.Add_Direction (0.95);
			glEnd ();
			glBegin (Render_Types [Data.Group_Size]);
			//std::cout << (int) (normal.Dot (pointing)) << std::endl;
			//if (normal.Dot (pointing) > 0)
			//{
				//std::cout << "---\n" << normal.Get_X_Angle () << ", " << normal.Get_Y_Angle ()
				//	<< std::endl;
				//normal.Set_Direction (1.0, 180 + normal.Get_X_Angle (),
				//	normal.Get_Y_Angle () * -1);
				//float color [] = {1.0, 0.0, 0.0, 1.0};
				//glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
				//++flipped; // DELETE
				//if (Indigo::keys ['3']) skip = 3;
				//std::cout << normal.Get_X_Angle () << ", " << normal.Get_Y_Angle ()
				//	<< std::endl;
			//}
			/*Direction pointing = eye.Distance (points [Point].To_Direction ());
			if (!((pointing.Get_X_Angle () > ((int) (normal.Get_X_Angle () - 90) % 360)
				|| pointing.Get_X_Angle () < ((int) (normal.Get_X_Angle () + 90) % 360))
				&& (pointing.Get_Y_Angle () > ((int) (normal.Get_Y_Angle () - 90) % 360)
				|| pointing.Get_Y_Angle () < ((int) (normal.Get_Y_Angle () + 90) % 360))))
			{
				normal.Add_Direction (0.0, 180.0, 180.0);
			}*/
			glNormal3f (normal.Get_X (), normal.Get_Y (), normal.Get_Z ());
		}
		if (skip > 0)	{
			skip--;
			continue;
		}
		Vertex Cursor = points [Point];
		glVertex3f (Cursor.X, Cursor.Y, Cursor.Z);
	}
	std::cout << "Flipped " << flipped * 100 / points.size () << "%" << std::endl; // DELETE
	glEnd ();
	glPopMatrix ();
	return;
}


// Places the object at the X, Y, and Z coordinates
void Object::Place (const float& x, const float& y, const float& z)
{
	X = x;
	Y = y;
	Z = z;
	return;
}


// Moves the object by X, Y, and Z relatively
void Object::Move (const float& x, const float& y, const float& z)
{
	X += x;
	Y += y;
	Z += z;
	return;
}


// The OpenGL draw mode for each render type.
const int Object::Render_Types [5] = {GL_TRIANGLE_STRIP, 0, 0, GL_TRIANGLES, GL_QUADS};