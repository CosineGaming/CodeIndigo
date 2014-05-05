// Animates variables steadily

#pragma once

#include <vector>
#include "Mesh.h"
#include "Object.h"


class Animation
{

public:

	// Create a new float animation to be animated each frame given a pointer, a final value, and a number of frames
	Animation(float * original=nullptr, float finished=0, int milliseconds=-1);
	// Construct an animation for tweening position of an object
	Animation(Object * original, float x, float y, float z, int frames);
	// Destroy an animation and remove it from the update list
	~Animation(void);
	// Update each animation in animations
	void Update(float time);
	// The ID for storing in the world
	int ID;

private:

	// The variable to affect
	float * change;
	// What the variable's value should change by each second. In seconds to increase float precision.
	float each;
	// Indigo::Elapsed point in which change will have reached the final variable
	int end_time;
	// Stores all animations to be updated for this constructor
	std::vector<Animation> animations;

};
