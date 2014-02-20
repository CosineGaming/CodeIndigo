// Animates variables steadily

#include "Animation.h"
#include <iostream>


// Create a new, blank animation to be assigned to
Animation::Animation(float * original, float finished, int frames)
{
	change = original;
	each = (finished - *original) / frames;
	remaining_frames = frames;
}


// Construct an animation based on tweening a shape
Animation::Animation(Mesh * original, Mesh finished, int frames)
{
	for (int point = 0; point < original->Size(); ++point)
	{
		animations.push_back(Animation(&((*original)[point].X), finished[point].X, frames));
		animations.push_back(Animation(&((*original)[point].Y), finished[point].Y, frames));
		animations.push_back(Animation(&((*original)[point].Z), finished[point].Z, frames));
	}
}


// Construct an animation for tweening position of an object
Animation::Animation(Object * original, float x, float y, float z, int frames)
{
	animations.push_back(Animation(&(original->X), x, frames));
	animations.push_back(Animation(&(original->Y), y, frames));
	animations.push_back(Animation(&(original->Z), z, frames));
}


// Destroy an animation and remove it from the update list
Animation::~Animation(void)
{
	
}


// Update each animation in animations
void Animation::Update(void)
{
	for (std::size_t animation = 0; animation < animations.size(); ++animation)
	{
		if (animations[animation].remaining_frames != 0)
		{
			*animations[animation].change += each;
			--animations[animation].remaining_frames;
		}
	}
}