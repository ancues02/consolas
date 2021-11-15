#pragma once

#include <SDL_events.h>

class SDL_Listener {
public:
	static void notify(SDL_Event* evt);
};
