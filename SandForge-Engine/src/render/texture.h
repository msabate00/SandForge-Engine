#pragma once
#include <app/defs.h>


struct Texture2D {
	uint id = 0; int w = 0, h = 0;
	bool Load(const char* path); void Destroy();
};