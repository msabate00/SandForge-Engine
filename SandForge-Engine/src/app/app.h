#pragma once
#include "app/defs.h"

class GLFWwindow;

class Engine;
class Renderer;
class Input;

class App
{
public:

	// Constructor
	App(int argc, char* args[]);

	// Destructor
	virtual ~App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

public:
	Engine* engine;
	Renderer* renderer;
	Input* input;



	Vec2<int> windowSize{ 1280, 720 };
	Vec2<int> gridSize{ 320, 180 };

private:
	int argc;
	char** args;

	

	GLFWwindow* window;

};

extern App* app;