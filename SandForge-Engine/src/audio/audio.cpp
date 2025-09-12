#define MINIAUDIO_IMPLEMENTATION
#include "audio.h" 
#include "app/app.h"
#include "app/defs.h"
#include "core/engine.h"
#include <core/utils.h>

Audio::Audio(App* app, bool start_enabled) : Module(app, start_enabled) {};
Audio::~Audio() = default;


bool Audio::Awake() {

	bool ret = (ma_engine_init(NULL, &ma_eng) == MA_SUCCESS);
	
    LoadAudiosInMemory();
	
	return ret;

}
bool Audio::Start() {

	return true;
}
bool Audio::PreUpdate() { return true; }

bool Audio::Update(float dt) {

    std::vector<AudioEvent> evs;
    if (app->engine->TakeAudioEvents(evs)) {
        for (const auto& e : evs) {

            float x01 = float(e.x) / float(app->gridSize.x);
            float y01 = float(e.y) / float(app->gridSize.y);
            switch (e.type) {
            case AudioEvent::Type::Ignite: Play("ignite", x01, y01, 0.9f); break;
            case AudioEvent::Type::Paint:  Play("paint", x01, y01, 0.5f); break;
            }
        }
    }

	return true;
}
bool Audio::PostUpdate() { return true; }

bool Audio::CleanUp() {


	return true;

}

void Audio::Play(const std::string& key, float x01, float y01, float vol)
{
    auto it = sfx.find(key);
    if (it == sfx.end() || it->second.voices.empty()) return;

    x01 = clamp01(x01); y01 = clamp01(y01);
    float pan = x01 * 2.f - 1.f;
    float att = 1.f - 0.6f * y01;
    float gain = clamp01(vol * att);

    Sfx& s = it->second;
    ma_sound* v = s.voices[s.cursor];
    s.cursor = (s.cursor + 1) % s.voices.size();

    ma_sound_stop(v);
    ma_sound_seek_to_pcm_frame(v, 0);
    ma_sound_set_pan(v, pan);
    ma_sound_set_volume(v, gain);
    ma_sound_start(v);
}

void Audio::LoadAudiosInMemory()
{
	Load("ignite", AUDIO_DIR  "/ignite.wav", 16);
	Load("paint", AUDIO_DIR  "/paint.wav", 8);
}

bool Audio::Load(const std::string& key, const char* path, int voices)
{
    Sfx s;
    s.voices.reserve(size_t(voices));
    for (int i = 0; i < voices; ++i) {
        auto* v = new ma_sound;
        if (ma_sound_init_from_file(&ma_eng, path,
            MA_SOUND_FLAG_DECODE, NULL, NULL, v) != MA_SUCCESS) {
            delete v;
            return false;
        }
        ma_sound_set_spatialization_enabled(v, MA_FALSE);
        s.voices.push_back(v);
    }
    sfx[key] = std::move(s);
    return true;
}


