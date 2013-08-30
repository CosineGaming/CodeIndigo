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


Object::Object (const Object& object)
{
	// Copy an object, used as main constructor
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


Object::~Object (void)
{
	// Destroys the object
	return;
}


void Object::Render (void) const
{
	// Renders the object
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
		Vertex Cursor = points [Point];
		glVertex3f (Cursor.X, Cursor.Y, Cursor.Z);
	}
	glEnd ();
	glPopMatrix ();
	return;
}


void Object::Place (const float& x, const float& y, const float& z)
{
	// Places the object at the X, Y, and Z coordinates
	X = x;
	Y = y;
	Z = z;
	return;
}


void Object::Move (const float& x, const float& y, const float& z)
{
	// Moves the object by X, Y, and Z relatively
	X += x;
	Y += y;
	Z += z;
	return;
}


const int Object::Render_Types [5] = {GL_TRIANGLE_STRIP, 0, 0, GL_TRIANGLES, GL_QUADS};