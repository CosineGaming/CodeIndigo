// Graphical objects to be displayed on the screen,
// along with useful functions for drawing and warping them

#include "Object.h"
#include <stdlib.h>
#include "glut.h"


Object::Object (const float& x, const float& y, const float& z,
	const Mesh& mesh, float *color, float shine,
	void (*update_function) (const int& frame, const Object& self),
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
	float full_array [] = {1.0, 1.0, 1.0, 1.0};
	glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, object_color ? object_color : full_array);
	glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, full_array);
	glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, object_shine);
	glPushMatrix ();
	glTranslatef (X, Y, Z);
	std::vector <Vertex> points = Data.Get_Vertices ();
	glBegin (Render_Types [Data.Group_Size]);
	for (int Point=0; Point<points.size (); Point++)
	{
		// When each polygon is finished, calculate a light normal
		if ((Point + 1) % (Data.Group_Size == 0 ? 3 : Data.Group_Size) == 0)
		{
			Vertex two = points [Point - 1] - points [Point - 2];
			Vertex three = points [Point] - points [Point - 2];
			glNormal3f (two.Z * three.Y - two.Y * three.Z,
				two.X * three.Z - two.Z * three.X,
				two.Y * three.X - two.X * three.Y);
		}
		Vertex Cursor = points [Point];
		glVertex3f (Cursor.X, Cursor.Y, Cursor.Z);
	}
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