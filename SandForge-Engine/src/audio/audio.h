#pragma once
#include "app/module.h"
#include <vector>
#include <cstdint>
#include <core/material.h>
#include <unordered_map>
#include "../../third_party/miniaudio/miniaudio.h"


struct Vertex;

struct AudioEvent {
    enum Type { Ignite, Paint };
    Type type;
    int x, y;
};

struct Sfx {
    std::vector<ma_sound*> voices;
    size_t cursor = 0;
};


class Audio : public Module {
public:
    Audio(App* app, bool start_enabled = true);
    virtual ~Audio();

    bool Awake();
    bool Start();
    bool PreUpdate();
    bool Update(float dt);
    bool PostUpdate();

    bool CleanUp();

    void Play(const std::string& key, float x01, float y01, float vol = 1.0f);

private:

    void LoadAudiosInMemory();
    bool Load(const std::string& key, const char* path, int voices = 8);


public:


private:
    ma_engine ma_eng{};
    std::unordered_map<std::string, Sfx> sfx;

};
