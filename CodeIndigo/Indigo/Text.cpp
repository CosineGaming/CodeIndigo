#include "Text.h"
#include "Indigo.h"


// Create new Text based off of the text
Text::Text(const float x, const float y, char * display, float * color, void * font, const int frames_to_last, void(*finished)(void))
{
  X = x;
  Y = y;
  text = display;
  text_color = color;
  if (!text_color)
  {
    text_color = Indigo::White_Color;
  }
  typeset = font;
  last = frames_to_last;
  done = finished;
  return;
}


// Create new Text based off of another Text object
Text::Text(const Text& copy)
{
  X = copy.X;
  Y = copy.Y;
  text = copy.text;
  text_color = copy.text_color;
  typeset = copy.typeset;
  last = copy.last;
  done = copy.done;
  return;
}


// Destroy the text
Text::~Text()
{
  return;
}


// Render the text
bool Text::Render(void)
{
  if (last != 0)
  {
    glRasterPos2f(X, Y);
    glColor3f(text_color[0], text_color[1], text_color[2]);
    for (int character = 0; text[character] != '\0'; ++character)
    {
      glutBitmapCharacter(typeset, text[character]);
    }
    if (last > 0)
    {
      last--;
    }
    return false;
  }
  else
  {
    if (done)
    {
      done();
    }
    return true;
  }
}
