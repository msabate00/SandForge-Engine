#include "input.h"
#include "../core/engine.h"
#include "../core/material.h"
#include "../app/app.h"
#include <algorithm>

Input::Input(App* app, bool start_enabled) : Module(app, start_enabled) {};
Input::~Input() = default;

bool Input::Awake() { return true; }
bool Input::Start() { return true; }
bool Input::PreUpdate() { return true; }
bool Input::Update(float) { return true; }
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
    glfwGetCursorPos(window, &mx, &my);
}

void Input::EndFrame() {


    scrollYSteps = (int)scrollYAccum;
    scrollYAccum -= (double)scrollYSteps;

    std::fill(std::begin(pressed), std::end(pressed), false);
    std::fill(std::begin(released), std::end(released), false);
    std::fill(std::begin(mouseUp), std::end(mouseUp), false);

}

void Input::ProcessBindings(Material& brushMat, int& brushSize) {
    if (this->KeyDown(GLFW_KEY_1)) brushMat = Material::Sand;
    if (this->KeyDown(GLFW_KEY_2)) brushMat = Material::Water;
    if (this->KeyDown(GLFW_KEY_3)) brushMat = Material::Stone;
    if (this->KeyDown(GLFW_KEY_4)) brushMat = Material::Wood;
    if (this->KeyDown(GLFW_KEY_5)) brushMat = Material::Fire;
    if (this->KeyDown(GLFW_KEY_6)) brushMat = Material::Smoke;
    if (this->KeyDown(GLFW_KEY_9)) brushMat = Material::Empty;

    if (this->KeyDown(GLFW_KEY_P)) app->engine->paused = !app->engine->paused;
    if (this->KeyDown(GLFW_KEY_N)) app->engine->stepOnce = true;

    if (this->MouseDown(GLFW_MOUSE_BUTTON_1)) {
        app->engine->Paint(MouseX(), MouseY(), brushMat, brushSize);
    }
    if (this->MouseUp(GLFW_MOUSE_BUTTON_1)) {
        app->engine->StopPaint();
    }

    if (this->ScrollSteps() != 0) {
        brushSize += this->ScrollSteps();
        brushSize = std::max(1, std::min(64, brushSize));
    }
}

void Input::SKey(GLFWwindow* w, int key, int, int action, int) {
    if (key < 0 || key >= 512) return;
    auto* self = static_cast<Input*>(glfwGetWindowUserPointer(w));
    if (!self) return;

    switch (action) {
    case GLFW_PRESS:
        self->keys[key] = true;
        self->pressed[key] = true;
        break;
    case GLFW_REPEAT:
        self->keys[key] = true;
        break;
    case GLFW_RELEASE:
        self->keys[key] = false;
        self->released[key] = true;
        break;
    }
}

void Input::SMouseBtn(GLFWwindow* w, int button, int action, int) {
    if (button < 0 || button >= 8) return;
    auto* self = static_cast<Input*>(glfwGetWindowUserPointer(w));
    if (!self) return;
    if (action == GLFW_PRESS) self->mouseDown[button] = true;
    else if (action == GLFW_RELEASE) {
        self->mouseDown[button] = false;
        self->mouseUp[button] = true;
    } 
}

void Input::SScroll(GLFWwindow* w, double, double yoff) {
    auto* self = static_cast<Input*>(glfwGetWindowUserPointer(w));
    if (!self) return;
    self->scrollYAccum += yoff;
}
