// Graphical objects to be displayed on the screen,
// along with useful functions for drawing and warping them

#include "GL/glew.h"

#include "Object.h"

#include "Indigo.h"

#include <iostream> // DELETE
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"


// Create an object given optional position, a mesh,
// and whether the object should render in wireframe
Object::Object(const float x, const float y, const float z, const Mesh& mesh, float *color, void(*update_function)(const float time, Object& self),
	const char * change_texture, const bool smooth, const Direction& towards, const bool world_collide, const float shine, const bool line)
{
	Place(x, y, z);
	Data = mesh;
	Is_Blank = mesh.Size() == 0;
	if (change_texture)
	{
		Data.Texture(change_texture);
	}
	if (!Is_Blank)
	{
		Data.Initialize();
	}
	Object_Color = color;
	Update_Function = update_function;
	Vertex_Normals = smooth;
	Facing = towards;
	Object_Shine = shine;
	Line = line;
	World_Collide = world_collide;
	User_Data = std::vector<float>();
	ID = -1;
	return;
}


// Copy an object, used as main constructor
Object::Object(const Object& object)
{
	Place(object.X, object.Y, object.Z);
	Data = object.Data;
	Object_Color = object.Object_Color;
	Update_Function = object.Update_Function;
	Vertex_Normals = object.Vertex_Normals;
	Facing = object.Facing;
	Object_Shine = object.Object_Shine;
	Line = object.Line;
	World_Collide = object.World_Collide;
	Is_Blank = object.Is_Blank;
	User_Data = object.User_Data;
	ID = object.ID;
	return;
}


// Destroys the object
Object::~Object(void)
{
	return;
}


// Updates the object, preparing for User-Defined Update_Function
void Object::Update(const float time)
{
	if (Update_Function)
	{
		Update_Function(time, *this);
	}
}


// Renders the object
void Object::Render(void) const
{
	if (Is_Blank)
	{
		return;
	}
	float full_array [] = {1.0, 1.0, 1.0, 1.0};
	if (glIsEnabled(GL_LIGHTING))
	{
		//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, // TODO: I200
		//	Object_Color ? Object_Color : full_array);
		//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, full_array);
		//glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, Object_Shine);
	}
	else
	{
		//float * color = Object_Color ? Object_Color : full_array; // TODO: I200
		//glColor3f(color[0], color[1], color[2]);
	}
	if (Line)
	{
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // TODO: I200
	}
	else
	{
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // TODO: I200
	}
	if (Data.Texture_ID != -1)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, Data.Texture_ID);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}
	//glPushMatrix();
	glm::mat4 modeling = glm::mat4(1);
	modeling = glm::translate(modeling, glm::vec3(X, Y, Z));
	modeling = glm::rotate(modeling, Facing.Get_X_Angle(), glm::vec3(0, -1, 0));
	modeling = glm::rotate(modeling, Facing.Get_Y_Angle(), glm::vec3(1, 0, 0));
	//glMultMatrixf(&modeling[0][0]); // TODO: I200
	//glBegin(render_types[Data.Group_Size]);
	//for (int point = 0; point<Data.Size(); ++point)
	//{
	//	Vertex normal;
	//	if (Vertex_Normals)
	//	{
	//		normal = Data.Smooth_Normal(point);
	//	}
	//	else
	//	{
	//		normal = Data.Flat_Normal(point);
	//	}
	//	Vertex Cursor = Data[point];
	//	if (Data.Texture_ID != -1)
	//	{
	//		Vertex coord = Data.Texture_Coordinate(point);
	//		glTexCoord2f(coord.X, coord.Y);
	//	}
	//	glNormal3f(normal.X, normal.Y, normal.Z);
	//	glVertex3f(Cursor.X, Cursor.Y, Cursor.Z);
	//}
	//glEnd();
	//glPopMatrix();
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, Data.Index);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glDrawArrays(render_types[Data.Group_Size], 0, Data.Size());
	glDisableVertexAttribArray(0);
	return;
}


// Places the object at the X, Y, and Z coordinates
void Object::Place(const float x, const float y, const float z)
{
	X = x;
	Y = y;
	Z = z;
	return;
}


// Moves the forward, side, and up based on the facing direction
void Object::Move(const float forward, const float side, const float up)
{
	Direction direction = Facing;
	direction.Set_Direction(forward, direction.Get_X_Angle(), 0.0);
	X += direction.Get_X();
	Y += direction.Get_Y();
	Z += direction.Get_Z();
	direction.Set_Direction(side, direction.Get_X_Angle() + 90.0, 0.0);
	X += direction.Get_X();
	Y += direction.Get_Y();
	Z += direction.Get_Z();
	direction.Set_Direction(up, 0.0, 90.0);
	X += direction.Get_X();
	Y += direction.Get_Y();
	Z += direction.Get_Z();
	return;
}


// Checks whether this object collides with another object
bool Object::Collide(const Object& object, const float add_x, const float add_y, const float add_z) const
{
	return(
		object.Data.Hitbox[0].X + object.X + add_x <= Data.Hitbox[1].X + X
		&& object.Data.Hitbox[0].Y + object.Y + add_y <= Data.Hitbox[1].Y + Y
		&& object.Data.Hitbox[0].Z + object.Z + add_z <= Data.Hitbox[1].Z + Z
		&& object.Data.Hitbox[1].X + object.X + add_x >= Data.Hitbox[0].X + X
		&& object.Data.Hitbox[1].Y + object.Y + add_y >= Data.Hitbox[0].Y + Y
		&& object.Data.Hitbox[1].Z + object.Z + add_z >= Data.Hitbox[0].Z + Z);
}


// Checks whether this object will ever be intersected by a direction
bool Object::Collide(const Direction& position, const Direction& direction) const
{

	Vertex * hitbox = const_cast<Vertex *>(Data.Hitbox);

	if (position.Get_Z() < Data.Hitbox[0].Z)
	{

		hitbox[0].X = Data.Hitbox[1].X;

		hitbox[1].X = Data.Hitbox[0].X;
		if (position.Get_X() > Data.Hitbox[0].X)
			hitbox[1].Z = Data.Hitbox[0].Z;

	}

	if (position.Get_X() > Data.Hitbox[1].X)
	{

		hitbox[0].X = Data.Hitbox[1].X;
		hitbox[0].Z = Data.Hitbox[1].Z;

		hitbox[1].X = Data.Hitbox[0].X;
		hitbox[1].Z = Data.Hitbox[0].Z;
		if (position.Get_Z() > Data.Hitbox[0].Z)
			hitbox[1].X = Data.Hitbox[1].X;

	}

	if (position.Get_Z() > Data.Hitbox[1].Z
		&& position.Get_X() > Data.Hitbox[0].X)
	{

		hitbox[0].X = Data.Hitbox[0].X;
		hitbox[0].Z = Data.Hitbox[1].Z;

		hitbox[1].X = Data.Hitbox[1].X;
		hitbox[1].Z = Data.Hitbox[0].Z;
		if (position.Get_X() < Data.Hitbox[1].X)
			hitbox[1].Z = Data.Hitbox[1].Z;

	}

	Direction least = position.Distance(hitbox[0].To_Direction());
	Direction most = position.Distance(hitbox[1].To_Direction());

	return direction.Get_X_Angle() >= least.Get_X_Angle()
		&& direction.Get_X_Angle() <= most.Get_X_Angle()
		&& direction.Get_Y_Angle() >= least.Get_Y_Angle()
		&& direction.Get_Y_Angle() <= most.Get_Y_Angle();

}


// Checks whether this vertex is withing this object
bool Object::Collide(const Vertex& vertex, const float add_x, const float add_y, const float add_z) const
{
	return(
		vertex.X + add_x <= Data.Hitbox[1].X + X
		&& vertex.Y + add_y <= Data.Hitbox[1].Y + Y
		&& vertex.Z + add_z <= Data.Hitbox[1].Z + Z
		&& vertex.X + add_x >= Data.Hitbox[0].X + X
		&& vertex.Y + add_y >= Data.Hitbox[0].Y + Y
		&& vertex.Z + add_z >= Data.Hitbox[0].Z + Z);
}


// Changes the relative hitbox for collision, set to 0 0 0 0 to make it uncollidable
void Object::Set_Hitbox(const float right, const float top, const float front, const float left, const float bottom, const float back)
{
	Data.Hitbox[1] = Vertex(right, top, front);
	Data.Hitbox[0] = Vertex(left, bottom, back);
}


// The OpenGL draw mode for each render type.
const int Object::render_types[5] = {GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, 0, GL_TRIANGLES, GL_QUADS};