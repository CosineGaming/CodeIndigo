// Graphical objects to be displayed on the screen,
// along with useful functions for drawing and warping them

#pragma once

#include "Mesh.h"
#include "Vertex.h"


class Object
{
public:
	// Create an object given optional position, a mesh,
	// and whether the object should render in wireframe
	Object (const float& x=0.0, const float& y=0.0, const float& z=0.0,
		const Mesh& mesh=Mesh(), float *color=nullptr, float shine=40.0,
		void (*update_function) (const int& frame, Object& self)=nullptr,
		const bool& line=false);
	// Copy an object
	Object (const Object& object);
	// Destroys the object
	~Object (void);

	// Renders the object
	void Render () const;
	// Places the object at the X, Y, and Z coordinates
	void Place (const float& x, const float& y=0.0, const float& z=0.0);
	// Moves the object by X, Y, and Z relatively
	void Move (const float& x, const float& y=0.0, const float& z=0.0);

	// Checked for whether to draw filled or lines
	bool Line;
	// Check if it's been fully initialized
	int Is_Blank;
	// An ID used for accessing and deleting the object
	int ID;
	// Called every time the World updates, set this as you please
	void (*Update) (const int& frame, Object& self);
	// X, Y, and Z position of center. Z is a constant if 2D.
	float X;
	float Y;
	float Z;
	// Relative position of points
	Mesh Data;
	// The color of the object
	float * object_color;
	// The shininess of the object, from 0 to 128
	float object_shine;
protected:
	// The OpenGL draw mode for each render type.
	static const int Render_Types [5];
private:
};