#pragma once
#include <cstdlib>
#include "glut.h"


class Text
{
public:

	// Create new text based off of the text. -1 is bottom, 1 is top. Text is placed with it's top-left corner at (text_x, text_y).
	// If time_to_last is negative, text won't disappear.
	Text(const float text_x=0, const float text_y=0, char * display="", float * color = nullptr, void * font = GLUT_BITMAP_HELVETICA_18, const int time_to_last = -1, void(*finished)(void)=nullptr);
	// Create new Text based off of another Text object
	Text(const Text& copy);
	// Destroy the text
	~Text();
	// Render the text, returns true if text should be deleted
	void Render(void) const;
	// The ID the world uses
	int ID;

private:

	// The text itself
	char * text;
	// The typeset to use
	void * typeset;
	// The frames until the text should go away
	int finished;
	// The position of the text in actual pixels
	float x;
	float y;
	// The color of the text
	float * text_color;
	// The function to call when the text goes away
	void(*done)(void);

};
