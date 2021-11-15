#ifdef PLATFORM_PC

#include "ImagePC.h"
#include "SDL_render.h"

Image::Image(SDL_Texture* surf) : imageb(surf) {
}

SDL_Texture* Image::getTexture() const {
	return imageb;
}

#endif