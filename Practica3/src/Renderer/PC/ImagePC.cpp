#ifdef PLATFORM_PC

#include "ImagePC.h"
#include "SDL_render.h"

Image::Image(SDL_Texture* surf) : _imageb(surf) {
}

Image::~Image()
{
	SDL_DestroyTexture(_imageb);
}

SDL_Texture* Image::getTexture() const {
	return _imageb;
}

#endif