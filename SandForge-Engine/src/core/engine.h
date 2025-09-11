#pragma once
#include "app/module.h"
#include <vector>
#include <cstdint>
#include "material.h"

struct AudioEvent {
    enum Type { Ignite, Paint };
    Type type;
    int x, y;
};


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

    const std::uint8_t* GetFrontPlane() { return mFront.data(); };


    bool tryMove(int x0, int y0, int x1, int y1, const Cell& c);
    bool trySwap(int x0, int y0, int x1, int y1, const Cell& c);

    void setCell(int x, int y, uint8 m);

    Material getCell(int x, int y);

    void Engine::Paint(int cx, int cy, Material m, int r);

private:

    void Step();

    int LinearIndex(int x, int y) const { return y * gridW + x; };

    bool InRange(int x, int y) const { return x >= 0 && x < gridW && y >= 0 && y < gridH; }

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

};