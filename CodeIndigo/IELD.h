#include "../Indigo/IndigoEngine.h"
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <thread>
#include <stdlib.h>
#include <Windows.h>

bool Load(const char * filename);

bool Save(const char * filename);

bool Save_For_Compile(const char * filename);

bool Add_And_Save(glm::vec3& position, const std::string& mesh, const std::string& texture);

void Fade_Text(float time, Object& self);

void GUI(float time);

void Mouse_Interact(int button, int state, float x, float y);

void Mouse_Look(int x, int y);

bool Text_Edit(int key, std::string& output);

void Print(const int handle, const std::string toprint);

void Key_Pressed(int key);

int main(int argc, char ** argv);
