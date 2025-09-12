#pragma once
#include "app/module.h"
#include <vector>
#include <cstdint>
#include "material.h"
#include "audio/audio.h"



class Engine : public Module {
public:
    Engine(App* app, bool start_enabled = true);
    virtual ~Engine();

    bool Awake();
    bool Start();
    bool PreUpdate();
    bool Update(float dt);
    bool PostUpdate();

    bool CleanUp();

    const uint8* GetFrontPlane() { return mFront.data(); };


    bool tryMove(int x0, int y0, int x1, int y1, const Cell& c);
    bool trySwap(int x0, int y0, int x1, int y1, const Cell& c);

    void SetCell(int x, int y, uint8 m);

    Cell GetCell(int x, int y) {
        return (InRange(x, y)) ? front[LinearIndex(x, y)] : Cell{ (uint8)Material::NullCell };
    }

    void Engine::Paint(int cx, int cy, Material m, int r);
    void Engine::StopPaint();

    bool InRange(int x, int y) const { return x >= 0 && x < gridW && y >= 0 && y < gridH; }
    bool Engine::randbit(int x, int y, int parity);

private:

    void Step();

    int LinearIndex(int x, int y) const { return y * gridW + x; };

    

public:
    bool paused = false;
    bool stepOnce = false;
    int parity = 0;

private:
    std::vector<Cell> front, back;
    std::vector<uint8> mFront, mBack;
    int gridW, gridH;

    float elapsedTimeSinceStep = 0;
    static constexpr float fixedTimeStep = 1.f / 120.f;

    AudioInstance paintInstance{};

};