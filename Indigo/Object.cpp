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
	Start_Index = 0;
	Length_Index = 0;
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
	Start_Index = object.Start_Index;
	Length_Index = object.Length_Index;
	Update_Function = object.Update_Function;
	Render_Function = object.Render_Function;
	Facing = object.Facing;
	Scale = object.Scale;
	World_Collide = object.World_Collide;
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

	if (Data.Size == 0)
	{
		return;
	}

	glm::mat4 modeling = glm::mat4(1);
	modeling = glm::translate(modeling, glm::vec3(X, Y, Z));
	modeling = glm::rotate(modeling, Facing.Get_X_Angle(), glm::vec3(0, -1, 0));
	modeling = glm::rotate(modeling, Facing.Get_Y_Angle(), glm::vec3(1, 0, 0));
	modeling = glm::scale(modeling, Scale);
	glUniformMatrix4fv(Indigo::Current_World.Shader_Location("M_Model", true), 1, GL_FALSE, &modeling[0][0]);
	glUniformMatrix4fv(Indigo::Current_World.Shader_Location("M_View", true), 1, GL_FALSE, &view[0][0]);
	glm::mat4 mvp = projection * view * modeling;
	glUniformMatrix4fv(Indigo::Current_World.Shader_Location("M_All", true), 1, GL_FALSE, &mvp[0][0]);

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
	glEnableVertexAttribArray(Indigo::Current_World.Shader_Location("V_Position"));
	glBindBuffer(GL_ARRAY_BUFFER, Data.Vertices_ID);
	glVertexAttribPointer(Indigo::Current_World.Shader_Location("V_Position"), 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Texture UVs
	glEnableVertexAttribArray(Indigo::Current_World.Shader_Location("V_UV"));
	glBindBuffer(GL_ARRAY_BUFFER, Data.UV_ID);
	glVertexAttribPointer(Indigo::Current_World.Shader_Location("V_UV"), 2, GL_FLOAT, GL_TRUE, 0, (void *) 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Light normals
	glEnableVertexAttribArray(Indigo::Current_World.Shader_Location("V_Normal"));
	glBindBuffer(GL_ARRAY_BUFFER, Data.Normals_ID);
	glVertexAttribPointer(Indigo::Current_World.Shader_Location("V_Normal"), 3, GL_FLOAT, GL_TRUE, 0, (void *) 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Data.Elements_ID);

	// Draw!
	glDrawElements(GL_TRIANGLES, Length_Index ? Length_Index : Data.Size, GL_UNSIGNED_SHORT, (void*) (Start_Index*sizeof(unsigned short)));

	// Finished
	//glDisableVertexAttribArray(Indigo::Current_World.Shader_Location("V_Position"));
	//glDisableVertexAttribArray(Indigo::Current_World.Shader_Location("V_UV"));
	//glDisableVertexAttribArray(Indigo::Current_World.Shader_Location("V_Normal"));

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


// Find the AABB needed for collisions
void Object::AABB(glm::vec3 out_less, glm::vec3 out_more) const
{

	if (Data.Size == 0)
	{
		out_more = glm::vec3(0, 0, 0);
		out_less = glm::vec3(0, 0, 0);
		return;
	}

	// Construct actual hitbox for this
	glm::vec3 hitbox[8];
	// Every possible combination of 2 is like counting in binary
	hitbox[0] = glm::vec3(Data.Hitbox[0].x, Data.Hitbox[0].y, Data.Hitbox[0].z);
	hitbox[1] = glm::vec3(Data.Hitbox[0].x, Data.Hitbox[0].y, Data.Hitbox[1].z);
	hitbox[2] = glm::vec3(Data.Hitbox[0].x, Data.Hitbox[1].y, Data.Hitbox[0].z);
	hitbox[3] = glm::vec3(Data.Hitbox[0].x, Data.Hitbox[1].y, Data.Hitbox[1].z);
	hitbox[4] = glm::vec3(Data.Hitbox[1].x, Data.Hitbox[0].y, Data.Hitbox[0].z);
	hitbox[5] = glm::vec3(Data.Hitbox[1].x, Data.Hitbox[0].y, Data.Hitbox[1].z);
	hitbox[6] = glm::vec3(Data.Hitbox[1].x, Data.Hitbox[1].y, Data.Hitbox[0].z);
	hitbox[7] = glm::vec3(Data.Hitbox[1].x, Data.Hitbox[1].y, Data.Hitbox[1].z);

	// Find the matrix to rotate / translate
	glm::mat4 modeling = glm::mat4(1);
	modeling = glm::translate(modeling, glm::vec3(X, Y, Z));
	modeling = glm::rotate(modeling, Facing.Get_X_Angle(), glm::vec3(0, -1, 0));
	modeling = glm::rotate(modeling, Facing.Get_Y_Angle(), glm::vec3(1, 0, 0));
	modeling = glm::scale(modeling, Scale);

	// Construct the new least / most
	glm::vec3 least;
	glm::vec3 most;

	for (int i = 0; i < 8; ++i)
	{
		glm::vec4 actual = modeling * glm::vec4(hitbox[i], 1);
		if (i == 0)
		{
			least.x = actual.x;
			least.y = actual.y;
			least.z = actual.z;
			most.x = actual.x;
			most.y = actual.y;
			most.z = actual.z;
		}
		else
		{
			if (actual.x < least.x)
			{
				least.x = actual.x;
			}
			if (actual.x > most.x)
			{
				most.x = actual.x;
			}
			if (actual.y < least.y)
			{
				least.y = actual.y;
			}
			if (actual.y > most.y)
			{
				most.y = actual.y;
			}
			if (actual.z < least.z)
			{
				least.z = actual.z;
			}
			if (actual.z > most.z)
			{
				most.z = actual.z;
			}
		}
	}

	out_less = least;
	out_more = most;

}


// Checks whether this object collides with another object
bool Object::Collide(const Object& object, const float add_x, const float add_y, const float add_z) const
{

	glm::vec3 least_this;
	glm::vec3 most_this;
	AABB(least_this, most_this);

	glm::vec3 least_that;
	glm::vec3 most_that;
	object.AABB(least_this, most_that);

	return (
		// Collides in some way
		add_x + least_this.x < most_that.x
		&& add_x + most_this.x >least_that.x
		&& add_y + least_this.y < most_that.y
		&& add_y + most_this.y >least_that.y
		&& add_z + least_this.z < most_that.z
		&& add_z + most_this.z >least_that.z
		// Not fully contained in object
		&& !(
		add_x + least_this.x >least_that.x
		&& add_x + most_this.x < most_that.x
		&& add_y + least_this.y >least_that.y
		&& add_y + most_this.y < most_that.y
		&& add_z + least_this.z >least_that.z
		&& add_z + most_this.z < most_that.z
		// Object not fully contained in this
		) && !(
		least_that.x > add_x + least_this.x
		&& most_that.x < add_x + most_this.x
		&&least_that.y > add_y + least_this.y
		&& most_that.y < add_y + most_this.y
		&&least_that.z > add_z + least_this.z
		&& most_that.z < add_z + most_this.z
		)
		);

}


// Checks whether this object was clicked on at world coord (x, y)
// Also can be used for collide with ray by setting camera_position to Camera.Place(start) Camera.Look_Towards(direction);
// Returns -1 if nothing's found, or the distance from the camera if something is.
float Object::Collide(const glm::vec2& position, const glm::mat4& camera_position) const
{

	// Construct actual hitbox for this
	glm::vec3 hitbox[8];
	// Every possible combination of 2 is like counting in binary
	hitbox[0] = glm::vec3(Data.Hitbox[0].x, Data.Hitbox[0].y, Data.Hitbox[0].z);
	hitbox[1] = glm::vec3(Data.Hitbox[0].x, Data.Hitbox[0].y, Data.Hitbox[1].z);
	hitbox[2] = glm::vec3(Data.Hitbox[0].x, Data.Hitbox[1].y, Data.Hitbox[0].z);
	hitbox[3] = glm::vec3(Data.Hitbox[0].x, Data.Hitbox[1].y, Data.Hitbox[1].z);
	hitbox[4] = glm::vec3(Data.Hitbox[1].x, Data.Hitbox[0].y, Data.Hitbox[0].z);
	hitbox[5] = glm::vec3(Data.Hitbox[1].x, Data.Hitbox[0].y, Data.Hitbox[1].z);
	hitbox[6] = glm::vec3(Data.Hitbox[1].x, Data.Hitbox[1].y, Data.Hitbox[0].z);
	hitbox[7] = glm::vec3(Data.Hitbox[1].x, Data.Hitbox[1].y, Data.Hitbox[1].z);

	// Find the matrix to project the hitbox to gather a new AABB in 2d camera space
	glm::mat4 MVP = camera_position;
	glm::mat4 modeling = glm::mat4(1);
	modeling = glm::translate(modeling, glm::vec3(X, Y, Z));
	modeling = glm::rotate(modeling, Facing.Get_X_Angle(), glm::vec3(0, -1, 0));
	modeling = glm::rotate(modeling, Facing.Get_Y_Angle(), glm::vec3(1, 0, 0));
	modeling = glm::scale(modeling, Scale);
	if (MVP == glm::mat4(0))
	{
		MVP = Indigo::Current_World.View.Project() * Indigo::Current_World.View.Look() * modeling;
	}
	else
	{
		MVP *= modeling;
	}

	// Project and find the min/max. Start with values we know won't screw it up because they're in hitbox.
	// 2d projection, except the min z is for return and to check wither it's in front.
	glm::vec3 min = glm::vec3(0, 0, 0);
	glm::vec2 max = glm::vec2(0, 0);
	for (int i = 0; i < 8; ++i)
	{
		glm::vec4 projected = MVP * glm::vec4(hitbox[i], 1);
		projected /= projected.w;
		// OpenGL needs things normalized to a cube. Indigo's standards are to keep things a rectangle to preserve aspect ratio. Standards must agree. Here we convert OpenGL standards to Indigo standards.
		projected.x *= Indigo::Aspect_Ratio;
		if (i == 0)
		{
			min.x = projected.x;
			min.y = projected.y;
			min.z = projected.z;
			max.x = projected.x;
			max.y = projected.y;
		}
		else
		{
			if (projected.x < min.x)
			{
				min.x = projected.x;
			}
			if (projected.x > max.x)
			{
				max.x = projected.x;
			}
			if (projected.y < min.y)
			{
				min.y = projected.y;
			}
			if (projected.y > max.y)
			{
				max.y = projected.y;
			}
			if (projected.z < min.z)
			{
				min.z = projected.z;
			}
		}
	}

	// Now we're ready! Easy operation with an AABB
	// < 0 is not a collision, since nothing on the screen would be behind the camera
	bool success = (min.z < 1 && min.x < position.x && max.x > position.x
		&& min.y < position.y && max.y > position.y && min.x);
	if (success)
	{
		return min.z;
	}
	else
	{
		return 2;
	}

}


// Checks whether this vertex is withing this object
bool Object::Collide(const glm::vec3& vertex, const float add_x, const float add_y, const float add_z) const
{
	glm::vec3 least;
	glm::vec3 most;
	AABB(least, most);

	return (
		add_x + vertex.x > least.x
		&& add_x + vertex.x < most.x
		&& add_y + vertex.y > least.y
		&& add_y + vertex.y < most.y
		&& add_z + vertex.z > least.z
		&& add_z + vertex.z < most.z
		);
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
