#pragma once
#include <glad/gl.h>
#include <string>
#include <app/defs.h>

std::string ReadTextFile(const char* path);

static float Clamp01(float v) { return v < 0.f ? 0.f : (v > 1.f ? 1.f : v); }

static void LogShader(GLuint s);
static void LogProgram(GLuint p);
uint MakeShader(unsigned int type, const char* src);
uint MakeProgram(const char* vs, const char* fs);

