#pragma once

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"


class Text
{
public:

	// Create new text based off of the text. -1 is bottom, 1 is top. Text is placed with it's top-left corner at (text_x, text_y).
	// If time_to_last is negative, text won't disappear.
	Text(const float text_x = 0, const float text_y = 0, char * display = "", glm::vec3 color = glm::vec3(1,1,1), void * font = nullptr, const float time_to_last = -1, void(*finished)(void) = nullptr);
	// Create new Text based off of another Text object
	Text(const Text& copy);
	// Destroy the text
	~Text();
	// Render the text, returns true if text should be deleted
	void Render(glm::mat4 project, glm::mat4 view) const;
	// The ID the world uses
	int ID;

private:

	// The text itself
	char * text;
	// The typeset to use
	void * typeset;
	// The time until the text should go away
	float finished;
	// The position of the text in actual pixels
	float x;
	float y;
	// The color of the text
	glm::vec3 text_color;
	// The function to call when the text goes away
	void(*done)(void);

};
