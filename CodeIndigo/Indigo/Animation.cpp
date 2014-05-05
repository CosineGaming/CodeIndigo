// Animates variables steadily

#include "Animation.h"

#include "Indigo.h"


// Create a new, blank animation to be assigned to
Animation::Animation(float * original, float finished, int milliseconds)
{
	change = original;
	each = (finished - *original) * 1000 / milliseconds;
	end_time = Indigo::Elapsed() + milliseconds;
	ID = -1;
	return;
}


// Construct an animation for tweening position of an object
Animation::Animation(Object * original, float x, float y, float z, int frames)
{
	animations.push_back(Animation(&(original->X), x, frames));
	animations.push_back(Animation(&(original->Y), y, frames));
	animations.push_back(Animation(&(original->Z), z, frames));
	ID = -1;
	return;
}


// Destroy an animation and remove it from the update list
Animation::~Animation(void)
{
	
}


// Update each animation in animations
void Animation::Update(float time)
{
	for (std::size_t animation = 0; animation < animations.size(); ++animation)
	{
		if (Indigo::Elapsed() >= animations[animation].end_time && animations[animation].change)
		{
			*animations[animation].change += each * time / 1000;
		}
	}
}