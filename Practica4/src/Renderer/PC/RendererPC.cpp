#ifdef PLATFORM_PC

#include "RendererPC.h"
#include <SDL.h>

#include "../../Platform/PC/PlatformPC.h"
#include <iostream>

int Renderer::_width = 1080;
int Renderer::_height = 720;
SDL_Window* Renderer::_window = nullptr;
SDL_Renderer* Renderer::_renderer = nullptr;

int Renderer::_numImages = 0;
Image** Renderer::_textures = nullptr;

bool Renderer::Init(bool fullscreen, int width, int height)
{
	if (fullscreen) {
		_window = SDL_CreateWindow("P2_VC", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			width, height, SDL_WINDOW_FULLSCREEN);
	}
	else {
		_window = SDL_CreateWindow("P2_VC", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			width, height, SDL_WINDOW_SHOWN);
	}
	if (!_window) {
		std::cerr << "Fallo iniciando la ventana de SDL" << std::endl;
		return false;
	}

	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!_renderer) {
		std::cerr << "Fallo iniciando el renderer de SDL" << std::endl;
		return false;
	}
	SDL_GetWindowSize(_window, &_width, &_height);
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

	return true;
}

void Renderer::Release()
{
	
	for (int i = 0; i < _numImages; ++i) {
		delete _textures[i];
		_textures[i] = nullptr;
	}
	delete []_textures;

	
	SDL_DestroyRenderer(_renderer);
	_renderer = nullptr;

	SDL_DestroyWindow(_window);
	_window = nullptr;

}

void Renderer::Clear(const Color& c)
{
	SDL_SetRenderDrawColor(_renderer, c.r, c.g, c.b, c.a);
	SDL_RenderClear(_renderer);
}

void Renderer::PutPixel(int x, int y, const Color& c)
{
	SDL_SetRenderDrawColor(_renderer, c.r, c.g, c.b, c.a);
	SDL_RenderDrawPoint(_renderer, x, y);
}

void Renderer::DrawLine(int x1, int y1, int x2, int y2, const Color& c)
{
	SDL_SetRenderDrawColor(_renderer, c.r, c.g, c.b, c.a);
	SDL_RenderDrawLine(_renderer, x1, y1, x2, y2);
}

void Renderer::DrawCircle(int cx, int cy, int rad, const Color& c)
{
	SDL_SetRenderDrawColor(_renderer, c.r, c.g, c.b, c.a);

	//ifdefstackoverflow 
	const int32_t diameter = (rad * 2);

	int32_t x = (rad - 1);
	int32_t y = 0;
	int32_t tx = 1;
	int32_t ty = 1;
	int32_t error = (tx - diameter);

	while (x >= y)
	{
		//  Each of the following renders an octant of the circle
		SDL_RenderDrawPoint(_renderer, cx + x, cy - y);
		SDL_RenderDrawPoint(_renderer, cx + x, cy + y);
		SDL_RenderDrawPoint(_renderer, cx - x, cy - y);
		SDL_RenderDrawPoint(_renderer, cx - x, cy + y);
		SDL_RenderDrawPoint(_renderer, cx + y, cy - x);
		SDL_RenderDrawPoint(_renderer, cx + y, cy + x);
		SDL_RenderDrawPoint(_renderer, cx - y, cy - x);
		SDL_RenderDrawPoint(_renderer, cx - y, cy + x);

		if (error <= 0)
		{
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0)
		{
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
	//endifstackoverflow
}

void Renderer::DrawImage(const Image &img, int x1, int y1, int x2, int y2) {
	SDL_Rect r = SDL_Rect{ x1, y1, x2, y2 };
	SDL_RenderCopy(_renderer, img.getTexture(), NULL, &r);
}

void Renderer::DrawImageColumn(const Image& img, int c, int x1, int y1, int x2, int y2) {
	int w, h;
	SDL_QueryTexture(img.getTexture(), NULL, NULL, &w, &h);	// Datos de la textura
	if (c < 0 || c >= w) return; // fallo
	SDL_Rect s = SDL_Rect{ c, 0, 1, h };
	SDL_Rect r = SDL_Rect{ x1, y1, x2, y2 };
	SDL_RenderCopy(_renderer, img.getTexture(), &s, &r);
}

void Renderer::DrawRect(int x, int y, int w, int h, const Color& c)
{
	SDL_SetRenderDrawColor(_renderer, c.r, c.g, c.b, c.a);
	SDL_Rect rect;
	rect.x = x; rect.y = y;
	rect.w = w; rect.h = h;
	SDL_RenderFillRect(_renderer, &rect);
}

void Renderer::Present()
{
	SDL_RenderPresent(_renderer);
}

bool Renderer::ReadImage(const char* name) {
	bool bigEndian = Platform::IsBigEndian();

	FILE* file = Platform::OpenFile(name, "rb");

	if (file == nullptr) return false; // Ha fallado la carga
	int w, h;
	if (fread(&_numImages, 4, 1, file) == 0) {
		Platform::CloseFile(name);
		return false;
	}
	if (!bigEndian) 
		_numImages = FLIPENDIAN_32((int)_numImages);

	_textures = new Image*[_numImages];

	for (int x = 0; x < _numImages && !feof(file); ++x) {
		if (fread(&w, 4, 1, file) == 0) break;
		if (fread(&h, 4, 1, file) == 0) break;
		if (!bigEndian) {
			w = FLIPENDIAN_32((int)w);
			h = FLIPENDIAN_32((int)h);
		}
		int32_t* imageb = new int32_t[w * h];
		for (int i = 0; i < w * h; ++i) {
			if (fread(&imageb[i], 4, 1, file) == 0) break;
			if (bigEndian) imageb[i] = FLIPENDIAN_32(imageb[i]);
			if ((imageb[i] & 0xff000000) != 0xff000000) imageb[i] = imageb[i] & 0x00fffff;
		}
		
		/*
			imageb, pixels de la imagen,
			w, ancho
			h, alto,
			32, depth, en este caso rgba
			4 * w,pitch, 4 bytes por pixel * pixeles por fila
			formato
		*/
		SDL_Surface* imageresult = SDL_CreateRGBSurfaceWithFormatFrom(imageb, w, h, 32, 4 * w, SDL_PIXELFORMAT_RGBA32);
		_textures[x] = new Image(SDL_CreateTextureFromSurface(_renderer, imageresult));
			
	}
	Platform::CloseFile(name);

	return true;
}

Image* Renderer::GetImage(int index) {
	return _textures[index];
}

#endif