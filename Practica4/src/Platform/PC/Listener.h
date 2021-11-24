#pragma once
#ifdef PLATFORM_PC
#include <SDL_events.h>

class SDL_Listener {
public:
	virtual void notify(SDL_Event* evt) = 0;
};
#endif