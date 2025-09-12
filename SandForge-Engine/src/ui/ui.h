#pragma once
#include "app/module.h"
#include <vector>
#include <cstdint>
#include <core/material.h>

struct Vertex;


class UI : public Module {
public:
    UI(App* app, bool start_enabled = true);
    virtual ~UI();

    bool Awake();
    bool Start();
    bool PreUpdate();
    bool Update(float dt);
    bool PostUpdate();

    bool CleanUp();

    void Begin(int viewW, int viewH);
    void End();
    void Draw(int& brushSize, Material& brushMat);

    void SetMouse(double x, double y, bool down);
    bool ConsumedMouse() const { return mouseConsumed; }


    bool Button(float x, float y, float w, float h,
        uint32_t rgba, uint32_t rgbaHover, uint32_t rgbaActive);
    bool Slider(float x, float y, float w, float h,
        float minv, float maxv, float& v, uint32_t track, uint32_t knob);



    void Rect(float x, float y, float w, float h, uint32_t rgba);

private:

    void Flush();



public:


private:

    unsigned int prog = 0, vao = 0, vbo = 0;
    int loc_uView = -1;

    std::vector<Vertex> verts;
    int vw = 0, vh = 0;



    double mx = 0.0, my = 0.0; bool md = false, mdPrev = false;
    bool mouseConsumed = false;

};

static inline uint32 RGBAu32(unsigned r, unsigned g, unsigned b, unsigned a = 255) {
    return (r & 255u) | ((g & 255u) << 8) | ((b & 255u) << 16) | ((a & 255u) << 24);
}

static inline uint32 MulRGBA(uint32_t c, float m) {
    unsigned r = (unsigned)((c) & 255u);
    unsigned g = (unsigned)((c >> 8) & 255u);
    unsigned b = (unsigned)((c >> 16) & 255u);
    unsigned a = (unsigned)((c >> 24) & 255u);
    auto sat = [](int v) { return (unsigned)(v < 0 ? 0 : v > 255 ? 255 : v); };
    r = sat(int(r * m)); g = sat(int(g * m)); b = sat(int(b * m));
    return (r) | (g << 8) | (b << 16) | (a << 24);
}