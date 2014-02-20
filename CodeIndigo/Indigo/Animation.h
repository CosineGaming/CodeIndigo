// Animates variables steadily

#pragma once

#include <vector>
#include "Mesh.h"
#include "Object.h"


class Animation
{

public:

	// Create a new float animation to be animated each frame given a pointer, a final value, and a number of frames
	Animation(float * original, float finished, int frames);
	// Construct an animation based on tweening a shape
	Animation(Mesh * original, Mesh finished, int frames);
	// Construct an animation for tweening position of an object
	Animation(Object * original, float x, float y, float z, int frames);
	// Destroy an animation and remove it from the update list
	~Animation(void);
	// Update each animation in animations
	void Update(void);

private:

	// The variable to affect
	float * change;
	// What the variable's value should change by each frame
	float each;
	// Remaining frames until change has reached the final variable
	int remaining_frames;
	// Stores all animations to be updated for this constructor
	std::vector<Animation> animations;

};