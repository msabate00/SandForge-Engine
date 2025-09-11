#include <array>
#include "material.h"
#include "engine.h"

static std::array<MatProps, 256> g_mat{};

const MatProps& matProps(uint8 id) { return g_mat[id]; }

void registerDefaultMaterials() {

    //MatProp                           //NAME      //Color             //Densidad  
    g_mat[(uint8)Material::Empty] = { "Empty",    {0,0,0,0,},           0,                  nullptr };
    g_mat[(uint8)Material::Sand] = { "Sand",     {217,191,77,255, 1},    3,                  nullptr };
    g_mat[(uint8)Material::Water] = { "Water",    {51,102,230,200, 1},    1,                  nullptr };
    g_mat[(uint8)Material::Stone] = { "Stone",    {128,128,140,255, 1},   255,              nullptr };
    g_mat[(uint8)Material::Wood] = { "Wood",     {142,86,55,255, 1},     255,                nullptr };
    g_mat[(uint8)Material::Fire] = { "Fire",     {255,35,1,255, 5.5f},      255,              nullptr };
    g_mat[(uint8)Material::Lava] = { "Lava",     {205,15,1,255, 15.5f},      255,              nullptr };
    g_mat[(uint8)Material::Smoke] = { "Smoke",    {28,13,2,255, 1},       255,                nullptr };
    g_mat[(uint8)Material::Steam] = { "Steam",    {200,200,200,255, 1},   255,               nullptr };


    g_mat[(uint8)Material::NpcCell] = { "Spawn",    {220, 40, 200,255},           0,                 nullptr };
}