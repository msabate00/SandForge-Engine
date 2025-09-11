#include "input.h"
#include "../core/engine.h"
#include "../core/material.h"
#include "../app/app.h"
#include <algorithm>


Input::Input(App* app, bool start_enabled) : Module(app, start_enabled) {};
Input::~Input() = default;

bool Input::Awake() {

    


    return true;

}
bool Input::Start() { return true; }
bool Input::PreUpdate() { return true; }

bool Input::Update(float dt) {



    return true;
}
bool Input::PostUpdate() { return true; }
bool Input::CleanUp() { return true; }

void Input::SetupWindow(GLFWwindow* window)
{
    this->window = window;
    glfwSetWindowUserPointer(this->window, this);
    glfwSetKeyCallback(this->window, &Input::SKey);
    glfwSetMouseButtonCallback(this->window, &Input::SMouseBtn);
    glfwSetScrollCallback(this->window, &Input::SScroll);
}

void Input::BeginFrame()
{
    std::copy(std::begin(keys), std::end(keys), std::begin(prevKeys));
    std::copy(std::begin(mouse), std::end(mouse), std::begin(prevMouse));

    glfwGetCursorPos(window, &mx, &my);

    scrollYSteps = (int)scrollYAccum;
    scrollYAccum -= (double)scrollYSteps;
}

void Input::EndFrame() {
   
}

void Input::ProcessBindings(Material& brushMat, int& brushSize) {

    if (this->KeyDown(GLFW_KEY_1)) brushMat = Material::Sand;
    if (this->KeyDown(GLFW_KEY_2)) brushMat = Material::Water;
    if (this->KeyDown(GLFW_KEY_3)) brushMat = Material::Stone;
    if (this->KeyDown(GLFW_KEY_4)) brushMat = Material::Wood;
    if (this->KeyDown(GLFW_KEY_5)) brushMat = Material::Fire;
    if (this->KeyDown(GLFW_KEY_6)) brushMat = Material::Smoke;
    if (this->KeyDown(GLFW_KEY_9)) brushMat = Material::Empty;
    
    if (this->KeyDown(GLFW_KEY_G)) {
        app->engine->setCell(30, 30, (uint8)Material::Sand);
    }

    if (this->KeyDown(GLFW_KEY_P)) app->engine->paused = !app->engine->paused;
    if (this->KeyDown(GLFW_KEY_N)) app->engine->stepOnce = true;


    if (this->MouseDown(GLFW_MOUSE_BUTTON_1)) {
        app->engine->Paint(MouseX(), MouseY(), brushMat, brushSize);
    };
   

    if (this->ScrollSteps() != 0) {
        brushSize += this->ScrollSteps();
        brushSize = std::max(1, std::min(64, brushSize));
    }
}


void Input::SKey(GLFWwindow* w, int key, int, int action, int) {
    if (key < 0 || key >= 512) return;
    auto* self = static_cast<Input*>(glfwGetWindowUserPointer(w));
    if (!self) return;
    if (action == GLFW_PRESS || action == GLFW_REPEAT) self->keys[key] = true;
    else if (action == GLFW_RELEASE) self->keys[key] = false;
}

void Input::SMouseBtn(GLFWwindow* w, int button, int action, int) {
    if (button < 0 || button >= 8) return;
    auto* self = static_cast<Input*>(glfwGetWindowUserPointer(w));
    if (!self) return;
    if (action == GLFW_PRESS) self->mouse[button] = true;
    else if (action == GLFW_RELEASE) self->mouse[button] = false;
}

void Input::SScroll(GLFWwindow* w, double, double yoff) {
    auto* self = static_cast<Input*>(glfwGetWindowUserPointer(w));
    if (!self) return;
    self->scrollYAccum += yoff;
}






