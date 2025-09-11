#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "app/app.h"

#include <chrono>
#include "app/timer.h"
#include "core/engine.h"
#include "core/input.h"
#include "render/renderer.h"





// Constructor
App::App(int argc, char* args[]) : argc(argc), args(args)
{
	engine = new Engine(this);
	renderer = new Renderer(this);
	input = new Input(this);
}

App::~App()
{

}


bool App::Awake()
{
	bool ret = true;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(windowSize.x, windowSize.y, "SandForge Engine", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) return -1;
	glfwSwapInterval(1);



	engine->Awake();
	registerDefaultMaterials();
	renderer->Awake();

	input->Awake();
	input->SetupWindow(window);


	return ret;
}

bool App::Start()
{
	bool ret = true;

	engine->Start();
	renderer->Start();


	return ret;
}

bool App::Update()
{


	Timer timer = Timer();
	double fpsTimer = 0.0;
	int frames = 0;

	Material brushMat = Material::Sand;
	int brushSize = 2;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float dt = timer.Read();
		timer.Start();

		input->BeginFrame();
		input->ProcessBindings(brushMat, brushSize); 
		engine->Update(dt);
		renderer->Update(dt);
		input->EndFrame();

		frames++;
		fpsTimer += dt;
		if (fpsTimer >= 1.0) {
			double fps = frames / fpsTimer;
			char buf[128];
			std::snprintf(buf, sizeof(buf), "SandForge Engine - %.1f FPS", fps);
			glfwSetWindowTitle(window, buf);
			fpsTimer = 0.0;
			frames = 0;
		}
		glfwSwapBuffers(window);
		glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
	}


	return false;
}

bool App::CleanUp()
{
	bool ret = true;


	return ret;
}