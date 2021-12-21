#pragma once
#include "..\..\Utils.h"
#include "ImagePC.h"

class SDL_Window;
class SDL_Renderer;

class Renderer {
public:
	static bool Init(bool fullscreen, int width, int height);
	static void Release();
	static void Clear(const Color& c);
	static void SplitClear(const Color& c1, const Color& c2);
	static void PutPixel(int x, int y, const Color& c);
	static void DrawLine(int x1, int y1, int x2, int y2, const Color& c);
	static void DrawCircle(int cx, int cy, int rad, const Color& c);
	static void DrawImage(const Image& img, int x1, int y1, int x2, int y2);
	static void DrawImageColumn(const Image& img, int c, int x1, int y1, int x2, int y2);
	static void DrawRect(int x, int y, int w, int h, const Color& c);
	static void Present();
	static int GetWidth() { return _width; }
	static int GetHeight() { return _height; }
	//P3
	static bool ReadImage(const char* name);
	static Image* GetImage(int index);

private:
	static int _width, _height;
	static SDL_Window* _window;
	static SDL_Renderer* _renderer;
	//P3
	static Image** _textures;
	static int _numImages;
	
};
