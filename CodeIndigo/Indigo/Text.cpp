#include "Text.h"
#include "Indigo.h"
#include <iostream>


// Create new text based off of the text. -1 is bottom, 1 is top. Text is placed with it's top-left corner at (text_x, text_y).
// If frames_to_last is negative, text won't disappear.
Text::Text(const float text_x, const float text_y, char * display, glm::vec3 color, void * font, const float time_to_last, void(*finished_function)(void))
{
	x = text_x;
	y = text_y;
	text = display;
	text_color = color;
	typeset = font;
	if (time_to_last >= 0)
	{
		finished = time_to_last + Indigo::Elapsed();
	}
	else
	{
		finished = time_to_last;
	}
	done = finished_function;
	ID = 0;
	return;
}


// Create new Text based off of another Text object
Text::Text(const Text& copy)
{
	x = copy.x;
	y = copy.y;
	text = copy.text;
	text_color = copy.text_color;
	typeset = copy.typeset;
	finished = copy.finished;
	done = copy.done;
	ID = copy.ID;
	return;
}


// Destroy the text
Text::~Text()
{
	return;
}


// Render the text
void Text::Render(glm::mat4 project, glm::mat4 view) const
{
	//glColor3f(text_color[0], text_color[1], text_color[2]);
	//glRasterPos2f(x, y);
	for (int character = 0; text[character] != '\0'; ++character)
	{
		//glutBitmapCharacter(typeset, text[character]);
	}
	if (Indigo::Elapsed() >= finished && finished >= 0)
	{
		if (done)
		{
			done();
		}
		Indigo::Current_World.Remove_Text(ID);
	}
}
