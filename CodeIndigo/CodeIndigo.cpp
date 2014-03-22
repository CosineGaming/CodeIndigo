#include "Indigo\IndigoEngine.h"
#include <iostream>

void update(int time, Object& self)
{
	if (Indigo::Keys['w'])
	{
		self.Move(0.2*time);
	}
	if (Indigo::Keys['s'])
	{
		self.Move(-0.2*time);
	}
	if (Indigo::Keys['a'])
	{
		self.Move(0, -0.2*time);
	}
	if (Indigo::Keys['d'])
	{
		self.Move(0, 0.2*time);
	}
	Indigo::Current_World.View.Place(self.X, self.Y + 0.75, self.Z);
	Indigo::Current_World.View.Eye = self.Facing;
	Indigo::FPS_Mouse(true, &self);
}

Object player = Object(0, 0.75, 0, Mesh(), nullptr, update);

void go_up(int time, Object& self)
{
	self.Y += 0.0005 * time;
	return;
}

void rotate(int time, Object& self)
{
	self.Facing.Add_Direction(0.0, 0.24 * time);
}

void shiny(int time, Object& self)
{
	static int direction = 1;
	self.Object_Shine += direction * time * 0.001;
	if (self.Object_Shine >= 128)
	{
		self.Object_Shine = 128;
		direction = -1;
	}
	if (self.Object_Shine <= 0)
	{
		self.Object_Shine = 0;
		direction = 1;
	}
}

void hit(int time, Object& self)
{
	static int animating = 0;
	if (Indigo::Left_Mouse && animating == 0)
	{
		animating = 1;
	}
	if (animating != 0)
	{
		if (animating == 1)
		{
			self.Facing.Set_X_Angle(self.Facing.Get_X_Angle() - 0.4 * time);
			if (!(self.Facing.Get_X_Angle() > 270 || self.Facing.Get_X_Angle() < 0))
			{
				animating = 2;
			}
		}
		else
		{
			self.Facing.Set_X_Angle(self.Facing.Get_X_Angle() + 0.4 * time);
			if (!(self.Facing.Get_X_Angle() > 270 || self.Facing.Get_X_Angle() < 0))
			{
				self.Facing.Set_X_Angle(0);
				animating = 0;
			}
		}
	}
}

int main(int argc, char ** argv)
{
	Indigo::Initialize(argc, argv, "My test!");
	Indigo::Current_World.Add_Skybox("Textures\\Skybox.bmp");
	Indigo::Current_World.Light_Setup.Add_Light(0, 0, 0, false);
	Indigo::Current_World.Add_Front_Object(Object(0.3, -0.2, 0.0, Mesh::Box(0.1, 0.1, 1.0), nullptr, hit));
	Indigo::Current_World.Add_Object(Object(0, 0, -5, Mesh::Cube(2), nullptr, rotate, nullptr, false, Direction(1,0,0), false, true, 0));
	Indigo::Current_World.Add_Object(Object(0, 0, 0, Mesh::Sphere(200, 5), Indigo::Blue_Color, shiny, nullptr, true, Direction(1, 0, 0), false, true, 0));
	Indigo::Current_World.Add_Object(Object(0, 5, 5, Mesh::Sphere(2), Indigo::Red_Color, go_up, "Textures\\texture.bmp", false));
	Indigo::Current_World.Add_Object(Object(0, -1.5, 0, Mesh::Rectangle(0.2,0.2), nullptr, nullptr, "Textures\\Floor.bmp", false, Direction(1,0,90)));
	Indigo::Current_World.Add_Object(player);
	Indigo::Run();
}