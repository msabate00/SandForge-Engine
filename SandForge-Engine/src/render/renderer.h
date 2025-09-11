#pragma once
#include "app/module.h"
#include "core/material.h"
#include <vector>
#include <cstdint>





class Renderer : public Module {
public:
    Renderer(App* app, bool start_enabled = true);
    virtual ~Renderer();

    bool Awake();
    bool Start();
    bool PreUpdate();
    bool Update(float dt);
    bool PostUpdate();

    bool CleanUp();

private:

    void DrawGrid(const std::vector<uint8_t>& indices, int w, int h, int viewW, int viewH);

    void uploadFullCPU(const std::uint8_t* img, int w, int h);
    void uploadRectPBO(const std::uint8_t* src, int rw, int rh, int x0, int y0,
        int texWNeeded, int texHNeeded);
    void ensureSceneTargets(int viewW, int viewH);
    void drawFullscreen();

public:

    void Draw(const std::uint8_t* planeM, int gridW, int gridH, int viewW, int viewH, int x0, int y0, int rw, int rh);

private:
    uint progGrid = 0, vao = 0, tex = 0;;
    int texW = 0, texH = 0;
    bool texValid = false;

    uint pbo[2] = { 0,0 };
    int pboIdx = 0;
    size_t pboCapacity = 0; // bytes

    uint paletteUBO = 0;

    int loc_uTex = -1;
    int loc_uGrid = -1;
    int loc_uView = -1;

    uint progThresh = 0, progBlur = 0, progComposite = 0;
    int loc_th_uScene = -1, loc_th_uThreshold = -1;
    int loc_bl_uTex = -1, loc_bl_uTexel = -1, loc_bl_uHorizontal = -1;
    int loc_cp_uScene = -1, loc_cp_uBloom = -1, loc_cp_uExposure = -1, loc_cp_uBloomStrength = -1;

    uint sceneFBO = 0, sceneTex = 0;
    uint pingFBO[2] = { 0,0 }, pingTex[2] = { 0,0 };
    int fboW = 0, fboH = 0;

    std::vector<uint8_t> scratch;     // full
    std::vector<uint8_t> scratchRect; // rect

};