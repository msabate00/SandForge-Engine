#include "Engine.h" 
#include "app/Module.h"  
#include "app/app.h"

Engine::Engine(App* app, bool start_enabled) : Module(app, start_enabled) {};
Engine::~Engine() = default;

bool Engine::Awake() { 

	gridW = app->gridSize.x;
	gridH = app->gridSize.y;

	front.assign(gridW * gridH, Cell{ (uint8)Material::Empty,0 });
	back.assign(gridW * gridH, Cell{ (uint8)Material::Empty,0 });
    mFront.assign(gridW * gridH, (uint8)Material::Empty);
    mBack.assign(gridW * gridH, (uint8)Material::Empty);

	return true;

}
bool Engine::Start() {
    return true; 
}
bool Engine::PreUpdate() { return true; }

bool Engine::Update(float dt) { 
	
	elapsedTimeSinceStep += dt;

    while (elapsedTimeSinceStep >= fixedTimeStep && (!paused || stepOnce)) {
        back = front;
        mBack = mFront;



        Step();
        


        front.swap(back);
        mFront.swap(mBack);

        elapsedTimeSinceStep -= fixedTimeStep;
        parity ^= 1;

        if (paused) { stepOnce = false; break; }
    }

    if (paused) elapsedTimeSinceStep = 0;



	return true;
}
bool Engine::PostUpdate() { return true; }
bool Engine::CleanUp() { return true; }



bool Engine::tryMove(int x0, int y0, int x1, int y1, const Cell& c)
{
    return false;
}

bool Engine::trySwap(int x0, int y0, int x1, int y1, const Cell& c)
{
    return false;
}

void Engine::setCell(int x, int y, uint8 m)
{
    if (!InRange(x, y)) return;
    int i = LinearIndex(x, y);
    uint8 prev = back[i].m;
    if (prev == m) return;

    front[i].m = m;  mFront[i] = m; 
    back[i].m = m;  mBack[i] = m;
}

Material Engine::getCell(int x, int y)
{
    return Material();
}

void Engine::Step() {

    for (int y = gridH - 1; y >= 0; --y) {
        bool l2r = ((y ^ parity) & 1);
        int x0 = l2r ? 0 : (gridW - 1);
        int x1 = l2r ? gridW : -1;
        int s = l2r ? 1 : -1;

        for (int x = x0; x != x1; x += s) {
            const Cell c = front[LinearIndex(x, y)];
            if (c.m == (uint8)Material::Empty) continue;
            const MatProps& mp = matProps(c.m);
            if (mp.update) mp.update(*this, x, y, c);
        }
    }

}

void Engine::Paint(int cx, int cy, Material m, int r) {

    cx = int((cx / double(app->windowSize.x)) * gridW);
    cy = int((cy / double(app->windowSize.y)) * gridH);


    int r2 = r * r;
    int xmin = std::max(0, cx - r), xmax = std::fmin(gridW - 1, cx + r);
    int ymin = std::max(0, cy - r), ymax = std::fmin(gridH - 1, cy + r);



    for (int y = ymin; y <= ymax; ++y)
        for (int x = xmin; x <= xmax; ++x) {
            int dx = x - cx, dy = y - cy;
            if (dx * dx + dy * dy <= r2) {
                int i = LinearIndex(x, y);
                front[i].m = (uint8)m;   // efecto inmediato
                mFront[i] = (uint8)m;    // SoA inmediato
                //markDirty(x, y);
            }
        }
   /* markDirtyRect(xmin, ymin, xmax, ymax);
    audioEvents.push_back({ AudioEvent::Type::Paint, cx, cy });*/
}