#pragma once
#include "app/module.h"
#include <cstdint>
#include <GLFW/glfw3.h>
#include "material.h"



class Input : public Module {
public:
    Input(App* app, bool start_enabled = true);
    virtual ~Input();

    bool Awake();
    bool Start();
    bool PreUpdate();
    bool Update(float dt);
    bool PostUpdate();

    bool CleanUp();

    void SetupWindow(GLFWwindow* window);

    void BeginFrame();
    void EndFrame();

    void ProcessBindings(Material& brushMat, int& brushSize);

    bool KeyDown(int key) const { return keys[key]; }
    bool KeyUp(int key) const { return keys[key] && !prevKeys[key]; }

    bool MouseDown(int button) const { return mouse[button]; }
    bool MouseClicked(int button) const { return mouse[button] && !prevMouse[button]; }

    double MouseX() const { return mx; }
    double MouseY() const { return my; }

    int ScrollSteps() const { return scrollYSteps; }

private:
    static void SKey(GLFWwindow* w, int key, int sc, int action, int mods);
    static void SMouseBtn(GLFWwindow* w, int button, int action, int mods);
    static void SScroll(GLFWwindow* w, double xoff, double yoff);


public:


private:
    GLFWwindow* window = nullptr;
    bool keys[512] = {};
    bool prevKeys[512] = {};

    bool mouse[8] = {};
    bool prevMouse[8] = {};

    double mx = 0.0, my = 0.0;
    double scrollYAccum = 0.0;
    int    scrollYSteps = 0;


};