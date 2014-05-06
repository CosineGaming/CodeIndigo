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
Object::Object(const float x, const float y, const float z, const Mesh& mesh, void(*update_function)(const float time, Object& self),
	const Direction& towards, const bool world_collide)
{
	Place(x, y, z);
	Data = mesh;
	Is_Blank = mesh.Size == 0;
	if (!Is_Blank)
	{
		//Data.Initialize();
	}
	Update_Function = update_function;
	Render_Function = nullptr;
	Facing = towards;
	Scale = glm::vec3(1, 1, 1);
	World_Collide = world_collide;
	User_Data = std::vector<float>();
	Shader_Argument_Names = std::vector<char *>();
	Shader_Arguments = std::vector<float>();
	ID = -1;
	return;
}


// Copy an object, used as main constructor
Object::Object(const Object& object)
{
	Place(object.X, object.Y, object.Z);
	Data = object.Data;
	Update_Function = object.Update_Function;
	Render_Function = object.Render_Function;
	Facing = object.Facing;
	Scale = object.Scale;
	World_Collide = object.World_Collide;
	Is_Blank = object.Is_Blank;
	User_Data = object.User_Data;
	Shader_Argument_Names = object.Shader_Argument_Names;
	Shader_Arguments = object.Shader_Arguments;
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
void Object::Render(const glm::mat4& projection, const glm::mat4& view, const bool lighting) const
{

	if (Render_Function)
	{
		Render_Function();
	}

	if (Is_Blank)
	{
		return;
	}

	glm::mat4 modeling = glm::mat4(1);
	modeling = glm::translate(modeling, glm::vec3(X, Y, Z));
	modeling = glm::rotate(modeling, Facing.Get_X_Angle(), glm::vec3(0, -1, 0));
	modeling = glm::rotate(modeling, Facing.Get_Y_Angle(), glm::vec3(1, 0, 0));
	modeling = glm::scale(modeling, Scale);
	glUniformMatrix4fv(Indigo::Current_World.Model_Matrix, 1, GL_FALSE, &modeling[0][0]);
	glUniformMatrix4fv(Indigo::Current_World.View_Matrix, 1, GL_FALSE, &view[0][0]);
	glm::mat4 mvp = projection * view * modeling;
	glUniformMatrix4fv(Indigo::Current_World.Matrix_Handle, 1, GL_FALSE, &mvp[0][0]);

	// Custom data
	for (int i = 0; i < Shader_Argument_Names.size(); ++i)
	{
		glUniform1f(Indigo::Current_World.Shader_Location(Shader_Argument_Names[i], true), Shader_Arguments[i]);
	}

	// Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Data.Texture_ID);
	glUniform1i(Indigo::Current_World.Shader_Location("F_Sampler", true), 0);

	// Shininess
	glUniform1f(Indigo::Current_World.Shader_Location("F_Shininess", true), Data.Shine);

	// Color
	glUniform4f(Indigo::Current_World.Shader_Location("F_Color", true), Data.Color.r, Data.Color.g, Data.Color.b, Data.Color.a);

	// Lighting enabled?
	glUniform1i(Indigo::Current_World.Shader_Location("F_Light_Enabled", true), lighting);

	// Vertices
	glEnableVertexAttribArray(Indigo::Current_World.Shader_Location("Position"));
	glBindBuffer(GL_ARRAY_BUFFER, Data.Vertices_ID);
	glVertexAttribPointer(Indigo::Current_World.Shader_Location("Position"), 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Texture UVs
	glEnableVertexAttribArray(Indigo::Current_World.Shader_Location("UV"));
	glBindBuffer(GL_ARRAY_BUFFER, Data.UV_ID);
	glVertexAttribPointer(Indigo::Current_World.Shader_Location("UV"), 2, GL_FLOAT, GL_TRUE, 0, (void *) 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Light normals
	glEnableVertexAttribArray(Indigo::Current_World.Shader_Location("Normal"));
	glBindBuffer(GL_ARRAY_BUFFER, Data.Normals_ID);
	glVertexAttribPointer(Indigo::Current_World.Shader_Location("Normal"), 3, GL_FLOAT, GL_TRUE, 0, (void *) 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Data.Elements_ID);

	// Draw!
	glDrawElements(GL_TRIANGLES, Data.Size, GL_UNSIGNED_SHORT, (void*) 0);

	// Finished
	glDisableVertexAttribArray(Indigo::Current_World.Shader_Location("Position"));
	glDisableVertexAttribArray(Indigo::Current_World.Shader_Location("UV"));
	glDisableVertexAttribArray(Indigo::Current_World.Shader_Location("Normal"));

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
	//return (Direction::Coordinates(X,Y,Z).Distance(Direction::Coordinates(object.X + add_x, object.Y + add_y, object.Z + add_z)).Get_Distance() < Data.Hitbox + object.Data.Hitbox);
	return (X + add_x + Data.Hitbox[0].x < object.X + object.Data.Hitbox[1].x
		&& X + add_x + Data.Hitbox[1].x > object.X + object.Data.Hitbox[0].x
		&& Y + add_y + Data.Hitbox[0].y < object.Y + object.Data.Hitbox[1].y
		&& Y + add_y + Data.Hitbox[1].y > object.Y + object.Data.Hitbox[0].y
		&& Z + add_z + Data.Hitbox[0].z < object.Z + object.Data.Hitbox[1].z
		&& Z + add_z + Data.Hitbox[1].z > object.Z + object.Data.Hitbox[0].z);
}


// Checks whether this object will ever be intersected by a direction
bool Object::Collide(const glm::vec3& position, const Direction& direction) const
{

	/*glm::vec3 * hitbox = const_cast<glm::vec3 *>(Data.Hitbox);

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
		&& direction.Get_Y_Angle() <= most.Get_Y_Angle();*/

	return false;

}


// Checks whether this vertex is withing this object
bool Object::Collide(const glm::vec3& vertex, const float add_x, const float add_y, const float add_z) const
{
	//return (Direction(X, Y, Z).Distance(Direction::Coordinates(vertex.x + add_x, vertex.y + add_y, vertex.z + add_z)).Get_Distance() < Data.Hitbox);
	return false;
}


// Changes the relative hitbox for preliminary collision, set to 0 to make it uncollidable
void Object::Set_Hitbox(const glm::vec3& least, const glm::vec3& most)
{
	Data.Hitbox[0] = least;
	Data.Hitbox[1] = most;
}


// Add a custom argument to send to the shader for this object.
void Object::Shader_Argument(char * argument, float value)
{
	Shader_Argument_Names.push_back(argument);
	Shader_Arguments.push_back(value);
}
