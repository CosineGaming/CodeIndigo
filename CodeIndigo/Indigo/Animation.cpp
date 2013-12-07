// Animates variables steadily

#include "Animation.h"


// Create a new, blank animation to be assigned to
Animation::Animation(float * original, float finished, int frames)
{
	change = original;
	each = (finished - *original) / frames;
	remaining_frames = frames;
	animations.push_back(*this);
	animations[animations.size() - 1].id = animations.size();
}


// Construct an animation based on tweening a shape
Animation::Animation(Mesh * original, Mesh finished, int frames)
{
	for (int point = 0; point < original->Size(); ++point)
	{
		Animation(&(*original)[point].X, finished[point].X, frames);
		Animation(&(*original)[point].Y, finished[point].Y, frames);
		Animation(&(*original)[point].Z, finished[point].Z, frames);
	}
}


// Construct an animation for tweening position of an object
Animation::Animation(Object * original, float X, float Y, float Z, int frames)
{
	Animation(&original->X, X, frames);
	Animation(&original->Y, Y, frames);
	Animation(&original->Z, Z, frames);
}


// Destroy an animation and remove it from the update list
Animation::~Animation(void)
{
}


// Remove an object from the update list
void Animation::Remove(void)
{
	animations.erase(animations.begin() + id);
}


// Update all animations; once per frame called by Indigo::Update
void Animation::Update_One(void)
{
	*change += each;
	remaining_frames--;
	if (remaining_frames == 0)
	{
		Remove();
	}
}


// Remove all animations and start anew for a new world or other purpose
void Animation::Clear(void)
{
	animations = std::vector<Animation>();
}


// Update each animation; called every frame by Indigo::Update
void Animation::Update(void)
{
	for (int animation = 0; animation < animations.size(); ++animation)
	{
		animations[animation].Update_One();
	}
}


std::vector<Animation> Animation::animations;