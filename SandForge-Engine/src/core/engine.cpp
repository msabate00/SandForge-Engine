#include "Engine.h" 
#include "app/Module.h"  
#include "app/app.h"
#include "input.h"

Engine::Engine(App* app, bool start_enabled) : Module(app, start_enabled) {};
Engine::~Engine() = default;




bool Engine::Awake() { 

	gridW = app->gridSize.x;
	gridH = app->gridSize.y;

    chunksW = (gridW + CHUNK_SIZE - 1) / CHUNK_SIZE;
    chunksH = (gridH + CHUNK_SIZE - 1) / CHUNK_SIZE;

	front.assign(gridW * gridH, Cell{ (uint8)Material::Empty,0 });
	back.assign(gridW * gridH, Cell{ (uint8)Material::Null,0 });
    mFront.assign(gridW * gridH, (uint8)Material::Empty);
    mBack.assign(gridW * gridH, (uint8)Material::Empty);

    chunkDirtyNow.assign(chunksW * chunksH, 1);
    chunkDirtyNext.assign(chunksW * chunksH, 0);
    chunkDirtyGPU.assign(chunksW * chunksH, 1);

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

    int nx = x0 + x1, ny = y0 + y1;
    if (!InRange(nx, ny)) return false;
    int si = LinearIndex(x0, y0), ni = LinearIndex(nx, ny);

    if (back[ni].m != (uint8)Material::Empty) return false;
   

    back[ni] = c;
    mBack[ni] = c.m;
    if (back[si].m == front[si].m) {
        back[si].m = (uint8)Material::Empty;
        mBack[si] = (uint8)Material::Empty;
    } 

    MarkChunkSim(x0, y0); MarkChunkSim(nx, ny);
    MarkChunkGPU(x0, y0); MarkChunkGPU(nx, ny);
    
    
    MarkChunksNeighborIfBorder(x0, y0);
    MarkChunksNeighborIfBorder(nx, ny);

    return true;
}

bool Engine::trySwap(int x0, int y0, int x1, int y1, const Cell& c)
{
    int nx = x0 + x1, ny = y0 + y1;
    if (!InRange(nx, ny)) return false;

    int si = LinearIndex(x0, y0);
    int ni = LinearIndex(nx, ny);
    if (si == ni) return false;

    const Cell& dst = front[ni];

    if (dst.m == (uint8)Material::Empty) return false; //No se puede intercambiar con empties
    if (matProps(c.m).density <= matProps(dst.m).density) return false; //No se puede intercambiar por densidad //Lo podria quitar realmetne

    back[ni] = c;
    back[si] = dst;
    mBack[ni] = c.m;
    mBack[si] = dst.m;

    MarkChunkSim(x0, y0); MarkChunkSim(nx, ny);
    MarkChunkGPU(x0, y0); MarkChunkGPU(nx, ny);


    MarkChunksNeighborIfBorder(x0, y0);
    MarkChunksNeighborIfBorder(nx, ny);

    return true;
}

void Engine::SetCell(int x, int y, uint8 m)
{
    if (!InRange(x, y)) return;
    int i = LinearIndex(x, y);
    if (back[i].m == m) return;

    back[i].m = m; 
    mBack[i] = m;

    MarkChunkSim(x, y);
    MarkChunkGPU(x, y);
    MarkChunksNeighborIfBorder(x, y);


}



void Engine::Step() {

    //FIX - EL ORDEN EN LO QUE SE LEEN LOS CHUNKS ES UN PROBLEMA, POR ESO LE PASA ESO A LA ARENA


    for (int cy = chunksH - 1; cy >= 0; cy--) {
        bool cl2r = ((cy ^ parity) & 1);
        int cx0 = cl2r ? 0 : (chunksW - 1);
        int cx1 = cl2r ? chunksW : -1;
        int cinc = cl2r ? 1 : -1;

        for (int cx = cx0; cx != cx1; cx += cinc) {
            int ci = ChunkLinearIndex(cx, cy);
            if (!chunkDirtyNow[ci]) continue;
            int cX, cY, cW, cH;
            GetChunkRect(ci, cX, cY, cW, cH);
            int x0 = cX;
            int y0 = cY;
            int x1 = cX + cW;
            int y1 = cY + cH;

            for (int y = y1 - 1; y >= y0; y--) {
                bool l2r = ((y ^ parity) & 1);
                int xs = l2r ? x0 : (x1 - 1);
                int xe = l2r ? x1 : (x0 - 1);
                int inc = l2r ? 1 : -1;

                for (int x = xs; x != xe; x += inc) {
                    const Cell c = front[LinearIndex(x, y)];
                    if (c.m == (uint8)Material::Empty) continue;

                    const MatProps& mp = matProps(c.m);
                    if (mp.update) mp.update(*this, x, y, c);

                }

            }

        }


    }




    //Old
    //for (int ci = 0; ci < chunkDirtyNow.size(); ci++) {
    //    if (!chunkDirtyNow[ci]) continue;
    //    int cX, cY, cW, cH;
    //    GetChunkRect(ci, cX, cY, cW, cH);
    //    int x0 = cX;
    //    int y0 = cY;
    //    int x1 = cX + cW;
    //    int y1 = cY + cH;

    //    for (int y = y1 - 1; y >= y0; y--) {
    //        bool l2r = ((y ^ parity) & 1);
    //        int xs = l2r ? x0 : (x1 - 1);
    //        int xe = l2r ? x1 : (x0 - 1);
    //        int inc = l2r ? 1 : -1;

    //        for (int x = xs; x != xe; x += inc) {
    //            const Cell c = front[LinearIndex(x, y)];
    //            if (c.m == (uint8)Material::Empty) continue;

    //            const MatProps& mp = matProps(c.m);
    //            if (mp.update) mp.update(*this, x, y, c);

    //        }

    //    }
    //}

    std::fill(chunkDirtyNow.begin(), chunkDirtyNow.end(), 0);
    chunkDirtyNow.swap(chunkDirtyNext);
    std::fill(chunkDirtyNext.begin(), chunkDirtyNext.end(), 0);
}

int Engine::ChunkIndexByCell(int x, int y) const
{

    if (!InRange(x, y)) return -1; //VIGILAR Hay que ver que pasa al devolver -1 en todos los casos
    int chunkX = x / CHUNK_SIZE;
    int chunkY = y / CHUNK_SIZE;

    return chunkY * chunksW + chunkX;
}

void Engine::MarkChunksInRect(int x, int y, int w, int h)
{
    int minX = std::max(0, x);
    int minY = std::max(0, y);
    int maxX = std::min(gridW, x + w);
    int maxY = std::min(gridH, y + h);

    //if (minX >= maxX || minY >= maxY) return; //VER QUE PASA SIN ESTO

    int minCX = minX / CHUNK_SIZE;
    int minCY = minY / CHUNK_SIZE;
    int maxCX = (maxX + CHUNK_SIZE - 1) / CHUNK_SIZE;
    int maxCY = (maxY + CHUNK_SIZE - 1) / CHUNK_SIZE;


    if (minCX < 0) minCX = 0;
    if (minCY < 0) minCY = 0;
    if (maxCX > chunksW) maxCX = chunksW;
    if (maxCY > chunksH) maxCY = chunksH;

    for (int cy = minCY; cy < maxCY; cy++) {
        for (int cx = minCX; cx < maxCX; cx++) {

            int ci = cy * chunksW + cx;

            MarkChunkSim(ci);
            MarkChunkGPU(ci);
            chunkDirtyNow[ci] = 1;

        }
    }
}

void Engine::MarkChunksNeighborIfBorder(int x, int y)
{
    int cx = x % CHUNK_SIZE, cy = y % CHUNK_SIZE;


    if (cx == 0) { MarkChunkSim(x - 1, y); } 
    else if (cx == CHUNK_SIZE - 1) { MarkChunkSim(x + 1, y); }

    if (cy == 0) { MarkChunkSim(x, y - 1); }
    else if (cy == CHUNK_SIZE - 1) { MarkChunkSim(x, y + 1); }

}

void Engine::GetChunkRect(int ci, int& x, int& y, int& w, int& h)
{
    if (ci < 0) { x = y = w = h = 0; return; }

    int cx = ci % chunksW;
    int cy = ci / chunksW;
    x = cx * CHUNK_SIZE;  
    y = cy * CHUNK_SIZE;
    w = std::min(gridW, x + CHUNK_SIZE) - x;
    h = std::min(gridH, y + CHUNK_SIZE) - y;
}

bool Engine::PopChunkDirtyGPURect(int& x, int& y, int& rw, int& rh)
{
    for (int cy = 0; cy < chunksH; cy++) {
        int cx = 0;
        while (cx < chunksW) {
            while (cx < chunksW && !chunkDirtyGPU[cy * chunksW + cx]) cx++;
            if (cx >= chunksW) break;
            int sx = cx;
            while (cx < chunksW && chunkDirtyGPU[cy * chunksW + cx]) {
                chunkDirtyGPU[cy * chunksW + cx] = 0;
                cx++;
            }
            x = sx * CHUNK_SIZE;
            y = cy * CHUNK_SIZE;
            rw = std::min(gridW - x, (cx - sx) * CHUNK_SIZE);
            rh = std::min(gridH - y, CHUNK_SIZE);
            return true;
        }
    }
    x = y = rw = rh = 0;
    return false;
}

void Engine::Paint(int cx, int cy, Material m, int r) {

    cx = int((cx / double(app->windowSize.x)) * gridW);
    cy = int((cy / double(app->windowSize.y)) * gridH);


    int r2 = r * r;
    int xmin = std::max(0, cx - r), xmax = std::min(gridW - 1, cx + r);
    int ymin = std::max(0, cy - r), ymax = std::min(gridH - 1, cy + r);



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
   /* markDirtyRect(xmin, ymin, xmax, ymax);*/
    MarkChunksInRect(xmin, ymin, xmax - xmin, ymax - ymin);


    if (!paintInstance) {
        paintInstance = app->audio->Play("paint", 0, 0, 1.0, true);
    }
    else {
        if (!app->audio->IsPlaying("paint", paintInstance)) {
            app->audio->Resume("paint", paintInstance);
        }
        app->audio->SetVoicePosition("paint", paintInstance, cx, 0);
    }
    

}

void Engine::StopPaint()
{
    if (paintInstance && app->audio->IsPlaying("paint", paintInstance)) {
        app->audio->Pause("paint", paintInstance);
    }
}

bool Engine::randbit(int x, int y, int parity) {
    uint32_t h = (uint32_t)(x * 374761393u) ^ (uint32_t)(y * 668265263u) ^ (uint32_t)(parity * 0x9E3779B9u);
    h ^= h >> 13; h *= 1274126177u; h ^= h >> 16;
    return (h & 1u) != 0u;
}