#pragma once
#ifdef PLATFORM_PC
#include "Listener.h"
#include "../../Input/Input.h"

void SDL_Listener::notify(SDL_Event* evt)
{
	// Añadir aqui todos los Listeners
	Input::beNotified(evt);
}
#endif