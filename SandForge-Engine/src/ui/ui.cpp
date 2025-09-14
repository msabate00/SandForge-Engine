#include "UI.h" 
#include <glad/gl.h>
#include "app/Module.h"  
#include "app/app.h"
#include "core/utils.h"
#include "core/engine.h"
#include "app/defs.h"


UI::UI(App* app, bool start_enabled) : Module(app, start_enabled) {};
UI::~UI() = default;


bool UI::Awake() {

	std::string vsSrc = readTextFile(SHADER_DIR "/ui.vs.glsl");
	std::string fsSrc = readTextFile(SHADER_DIR "/ui.fs.glsl");

	prog = makeProgram(vsSrc.c_str(), fsSrc.c_str());
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * 1024 * 1024, nullptr, GL_DYNAMIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);


	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, rgba));


	glBindVertexArray(0);
	loc_uView = glGetUniformLocation(prog, "uView");
    return true;

}
bool UI::Start() {

    return true;
}
bool UI::PreUpdate() { return true; }

bool UI::Update(float dt) {
    return true;
}
bool UI::PostUpdate() { return true; }
bool UI::CleanUp() { 


	if (vbo) glDeleteBuffers(1, &vbo);
	if (vao) glDeleteVertexArrays(1, &vao);
	if (prog) glDeleteProgram(prog);
	vbo = vao = prog = 0;

	return true;

}

void UI::Begin(int viewW, int viewH) {
	vw = viewW; vh = viewH; verts.clear();
}

void UI::Draw(int& brushSize, Material& brushMat) {

	const MatProps actualMat = matProps((uint8)brushMat);


	float pad = 8.0f, y = 8.0f, x = 8.0f, btn = 28.0f;
	auto makeBtn = [&](uint32 base) {
		uint32 h = MulRGBA(base, 1.15f), a = MulRGBA(base, 0.85f);
		bool clicked = Button(x, y, btn, btn, base, h, a);
		x += btn + 6.0f; return clicked;
		};


	for (int i = 0; i < 256; ++i) {
		const MatProps& mp = matProps((uint8)i);

		if (mp.name.length() > 0) {
			uint32 c = RGBAu32(mp.color.r, mp.color.g, mp.color.b, 230);
			if (makeBtn(c)) brushMat = (Material)i;
		}

	}



	x += 8.0f;

	if (app->engine->paused) {
		if (makeBtn(RGBAu32(250, 200, 200, 230))) app->engine->paused = false;
		if (makeBtn(RGBAu32(180, 220, 180, 230))) app->engine->stepOnce = true;
	}
	else {
		if (makeBtn(RGBAu32(200, 200, 200, 230))) app->engine->paused = true;
	}


	x += 12.0f; float bx = x, bw = 200.0f, bh = 20.0f; float v = (float)brushSize;
	Slider(bx, y + 4.0f, bw, bh, 1.0f, 64.0f, v, RGBAu32(90, 90, 100, 200), RGBAu32(230, 230, 240, 240));
	brushSize = (int)(v + 0.5f);

	Ring(mx, my, brushSize, 2, RGBAu32(actualMat.color.r, actualMat.color.g, actualMat.color.b, 100), 12);

	if (app->showChunks) {
		std::vector<uint> chunks = app->engine->GetChunks();
		for (int ci = 0; ci < chunks.size(); ci++) {
			int x, y, w, h;
			app->engine->GetChunkRect(ci, x, y, w, h);

			x = int((x / double(app->gridSize.x)) * app->windowSize.x);
			y = int((y / double(app->gridSize.y)) * app->windowSize.y);
			w = int((w / double(app->gridSize.x)) * app->windowSize.x);
			h = int((h / double(app->gridSize.y)) * app->windowSize.y);

			if (chunks[ci]) {
				RectBorders(x, y, w, h, 4, RGBAu32(230, 130, 130, 200));
			}
			else {
				RectBorders(x, y, w, h, 2, RGBAu32(230, 230, 230, 100));
			}
		}
	}
}

void UI::End() { Flush(); }

void UI::SetMouse(double x, double y, bool down) {
	mdPrev = md; md = down; mx = x; my = y; mouseConsumed = false;
}

void UI::Flush() {
	if (verts.empty()) return;
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // por si acaso
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glUseProgram(prog);
	glUniform2f(loc_uView, (float)vw, (float)vh);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, verts.size() * sizeof(Vertex), verts.data());
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_TRIANGLES, 0, (GLint)verts.size());
}



void UI::Rect(float x, float y, float w, float h, uint32 c) {
	Vertex v[6] = {
	{x, y, c}, {x + w, y, c}, {x + w, y + h, c},
	{x, y, c}, {x + w, y + h, c}, {x, y + h, c},
	};
	verts.insert(verts.end(), v, v + 6);
}

void UI::RectBorders(float x, float y, float w, float h, float t, uint32 rgba)
{

	float tM = (t / 2);
	Rect(x - tM, y - tM, w, tM, rgba);
	Rect(x - tM, (y+h) - tM, w, tM, rgba);

	Rect(x - tM, y - tM, tM, h, rgba);
	Rect((x+w) - tM, y- tM, tM, h, rgba);

}


bool UI::Button(float x, float y, float w, float h,
	uint32 c, uint32 cH, uint32 cA) {
	bool hover = (mx >= x && mx <= x + w && my >= y && my <= y + h);
	uint32 cc = hover ? (md ? cA : cH) : c;
	Rect(x, y, w, h, cc);


	bool clicked = hover && !md && mdPrev;
	if (hover && (md || clicked)) mouseConsumed = true;
	return clicked;
}


bool UI::Slider(float x, float y, float w, float h,
	float minv, float maxv, float& v,
	uint32 track, uint32 knob) {
	if (v < minv) v = minv; if (v > maxv) v = maxv;
	Rect(x, y + h * 0.4f, w, h * 0.2f, track);
	float t = (v - minv) / (maxv - minv);
	float kx = x + t * w;
	float kw = h;
	float kx0 = kx - kw * 0.5f;
	Rect(kx0, y, kw, h, knob);


	bool hover = (mx >= x && mx <= x + w && my >= y && my <= y + h);
	if (hover && md) { v = minv + float((mx - x) / w) * (maxv - minv); mouseConsumed = true; }
	return hover && !md && mdPrev; // soltado sobre el slider
}

void UI::Circle(float cx, float cy, float r, uint32 c, int segments) {

	float rx = r * (float)app->windowSize.x / (float)app->gridSize.x;
	float ry = r * (float)app->windowSize.y / (float)app->gridSize.y;

	if (rx <= 0 || ry <= 0) return;

	int n = segments > 0 ? segments : std::max(12, std::min(128, int((6.2831853f * std::max(rx, ry)) / 8.0f)));
	const float dA = 6.2831853f / n;

	verts.reserve(verts.size() + 3 * n);
	Vertex center{ cx, cy, c };

	float px = cx + rx, py = cy;
	for (int i = 1;i <= n;++i) {
		float a = i * dA, x = cx + std::cos(a) * rx, y = cy + std::sin(a) * ry;
		verts.push_back(center); verts.push_back({ px,py,c }); verts.push_back({ x,y,c });
		px = x; py = y;
	}
}


void UI::Ring(float cx, float cy, float r, float t, uint32 c, int segments) {

	float rx = r * (float)app->windowSize.x / (float)app->gridSize.x;
	float ry = r * (float)app->windowSize.y / (float)app->gridSize.y;

	if (t <= 0 || rx <= 0 || ry <= 0) return;

	float rx0 = std::max(0.0f, rx - t * 0.5f), ry0 = std::max(0.0f, ry - t * 0.5f);
	float rx1 = rx + t * 0.5f, ry1 = ry + t * 0.5f;
	int n = segments > 0 ? segments : std::max(12, std::min(128, int((6.2831853f * std::max(rx, ry)) / 8.0f)));
	float dA = 6.2831853f / n;
	float p0x = cx + rx0, p0y = cy, p1x = cx + rx1, p1y = cy;
	for (int i = 1;i <= n;++i) {
		float a = i * dA;
		float x0 = cx + std::cos(a) * rx0, y0 = cy + std::sin(a) * ry0;
		float x1 = cx + std::cos(a) * rx1, y1 = cy + std::sin(a) * ry1;
		verts.push_back({ p0x,p0y,c }); verts.push_back({ p1x,p1y,c }); verts.push_back({ x1,y1,c });
		verts.push_back({ p0x,p0y,c }); verts.push_back({ x1,y1,c });   verts.push_back({ x0,y0,c });
		p0x = x0; p0y = y0; p1x = x1; p1y = y1;
	}
}


