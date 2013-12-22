// Graphical objects to be displayed on the screen,
// along with useful functions for drawing and warping them

#include "Object.h"
#include "Indigo.h"
#include <stdlib.h>
#include <iostream> // DELETE
#include "glut.h"


// Create an object given optional position, a mesh,
// and whether the object should render in wireframe
Object::Object(const float& x, const float& y, const float& z,
	const Mesh& mesh, float *color, float shine,
	void(*update_function)(const int& frame, Object& self),
	const bool& line, const Direction& towards)
{
	Place(x, y, z);
	Data = mesh;
	object_color = color;
	object_shine = shine;
	Update = update_function;
	Line = line;
	facing = towards;
	Is_Blank = mesh.Size() == 0;
	ID = -1;
	return;
}


// Copy an object, used as main constructor
Object::Object(const Object& object)
{
	Place(object.X, object.Y, object.Z);
	Data = object.Data;
	object_color = object.object_color;
	object_shine = object.object_shine;
	Update = object.Update;
	Line = object.Line;
	facing = object.facing;
	Is_Blank = object.Is_Blank;
	ID = object.ID;
	return;
}


// Destroys the object
Object::~Object(void)
{
	return;
}


// Renders the object
void Object::Render(void) const
{
	float full_array [] = {1.0, 1.0, 1.0, 1.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,
		object_color ? object_color : full_array);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, full_array);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, object_shine);

	if (Line)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glRotatef()
	glTranslatef(X, Y, Z);

	std::vector <Vertex> points = Data.Get_Vertices();
	glBegin(Render_Types[Data.Group_Size]);
	
	for (int Point=0; Point<points.size(); Point++)
	{
		// When each polygon is finished, calculate a light normal
		Vertex normal = Data.Get_Normal(Point);
		//if (flip)
		//{
			glNormal3f(normal.X, normal.Y, normal.Z);
		//}
		//else
		//{
		//	glNormal3f(normal.X * -1, normal.Y * -1, normal.Z * -1);
		//}
		Vertex Cursor = points [Point];
		glVertex3f(Cursor.X + X, Cursor.Y + Y, Cursor.Z + Z);
	}
	//std::cout << "Flipped " << flipped * 100 / points.size() << "%" << std::endl; // DELETE
	glEnd();
	return;
}


// Places the object at the X, Y, and Z coordinates
void Object::Place(const float& x, const float& y, const float& z)
{
	X = x;
	Y = y;
	Z = z;
	return;
}


// Moves the object by X, Y, and Z relatively
void Object::Move(const float& x, const float& y, const float& z)
{
	X += x;
	Y += y;
	Z += z;
	return;
}


// Checks whether this object collides with another object
bool Object::Collide(const Object& object, const float add_x, const float add_y, const float add_z) const
{
	return(
		   object.Data.Hitbox [0].X + object.X <= Data.Hitbox [1].X + X
		&& object.Data.Hitbox [0].Y + object.Y <= Data.Hitbox [1].Y + Y
		&& object.Data.Hitbox [0].Z + object.Z <= Data.Hitbox [1].Z + Z
		&& object.Data.Hitbox [1].X + object.X >= Data.Hitbox [0].X + X
		&& object.Data.Hitbox [1].Y + object.Y >= Data.Hitbox [0].Y + Y
		&& object.Data.Hitbox [1].Z + object.Z >= Data.Hitbox [0].Z + Z);
}


// Checks whether this object will ever be intersected by a direction
bool Object::Collide(const Direction& position, const Direction& direction) const
{

	Vertex * hitbox = const_cast<Vertex *>(Data.Hitbox);

	if (position.Get_Z() < Data.Hitbox [0].Z)
	{

		hitbox [0].X = Data.Hitbox [1].X;

		hitbox [1].X = Data.Hitbox [0].X;
		if (position.Get_X() > Data.Hitbox [0].X)
			hitbox [1].Z = Data.Hitbox [0].Z;

	}

	if (position.Get_X() > Data.Hitbox [1].X)
	{

		hitbox [0].X = Data.Hitbox [1].X;
		hitbox [0].Z = Data.Hitbox [1].Z;

		hitbox [1].X = Data.Hitbox [0].X;
		hitbox [1].Z = Data.Hitbox [0].Z;
		if (position.Get_Z() > Data.Hitbox [0].Z)
			hitbox [1].X = Data.Hitbox [1].X;

	}

	if (position.Get_Z() > Data.Hitbox [1].Z
		&& position.Get_X() > Data.Hitbox [0].X)
	{

		hitbox [0].X = Data.Hitbox [0].X;
		hitbox [0].Z = Data.Hitbox [1].Z;

		hitbox [1].X = Data.Hitbox [1].X;
		hitbox [1].Z = Data.Hitbox [0].Z;
		if (position.Get_X() < Data.Hitbox [1].X)
			hitbox [1].Z = Data.Hitbox [1].Z;
		
	}

	Direction least = position.Distance(hitbox [0].To_Direction());
	Direction most = position.Distance(hitbox [1].To_Direction());

	return direction.Get_X_Angle() >= least.Get_X_Angle()
		&& direction.Get_X_Angle() <= most.Get_X_Angle()
		&& direction.Get_Y_Angle() >= least.Get_Y_Angle()
		&& direction.Get_Y_Angle() <= most.Get_Y_Angle();

}


// Checks whether this vertex is withing this object
bool Object::Collide(const Vertex& vertex, const float add_x, const float add_y, const float add_z) const
{
	return(
		   vertex.X <= Data.Hitbox [1].X + X
		&& vertex.Y <= Data.Hitbox [1].Y + Y
		&& vertex.Z <= Data.Hitbox [1].Z + Z
		&& vertex.X >= Data.Hitbox [0].X + X
		&& vertex.Y >= Data.Hitbox [0].Y + Y
		&& vertex.Z >= Data.Hitbox [0].Z + Z);
}


// Changes the relative hitbox for collision, set to 0 0 0 0 to make it uncollidable
void Object::Set_Hitbox(const float& right, const float& top, const float& front, const float& left, const float& bottom, const float& back)
{
	Data.Hitbox[1] = Vertex(right, top, front);
	Data.Hitbox[0] = Vertex(left, bottom, front);
}


// The OpenGL draw mode for each render type.
const int Object::Render_Types [5] = {GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, 0, GL_TRIANGLES, GL_QUADS};