#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "app/app.h"
#include <algorithm>
#include <chrono>
#include "app/timer.h"
#include "core/engine.h"
#include "core/input.h"
#include "render/renderer.h"
#include "ui/ui.h"


// Constructor
App::App(int argc, char* args[]) : argc(argc), args(args)
{
	engine = new Engine(this);
	renderer = new Renderer(this);
	input = new Input(this);
	ui = new UI(this);
	audio = new Audio(this);
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
	ui->Awake();

	input->Awake();
	input->SetupWindow(window);

	audio->Awake();

	


	return ret;
}

bool App::Start()
{
	bool ret = true;

	engine->Start();
	renderer->Start();
	input->Start();
	ui->Start();
	audio->Start(); //se puede quitar


	return ret;
}

bool App::Update()
{


	Timer timer = Timer();
	double fpsTimer = 0.0;
	frames = 0;

	Material brushMat = Material::Sand;
	int brushSize = 2;

	

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		dt = timer.Read();
		timer.Start();

		input->BeginFrame();
		

		//TODO - Mouse hide/show
		if (input->MouseY() > 50) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		ui->SetMouse(input->MouseX(), input->MouseY(), input->MouseDown(GLFW_MOUSE_BUTTON_1));
		ui->SetNoRender(true);
		ui->Begin(windowSize.x, windowSize.y);
		ui->Draw(brushSize, brushMat);
		ui->End();
		input->ProcessBindings(brushMat, brushSize);



		engine->Update(dt);

		audio->Update(dt); //se puede quitar

		renderer->Update(dt);
		
		ui->SetNoRender(false);
		ui->Begin(windowSize.x, windowSize.y);
		ui->Draw(brushSize, brushMat);
		ui->End();

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

	ui->CleanUp();
	input->CleanUp();
	renderer->CleanUp();
	engine->CleanUp();
	audio->CleanUp();

	return ret;
}


void App::SetCameraRect(float x, float y, float w, float h) {
	camera.size.x = std::fmax(1.f, w);
	camera.size.y = std::fmax(1.f, h);
	camera.pos.x = std::clamp(x, 0.f, (float)gridSize.x - camera.size.x);
	camera.pos.y = std::clamp(y, 0.f, (float)gridSize.y - camera.size.y);
}