#pragma once

#include "../../Platform/PC/Listener.h"

/*
	Clase para escuchar los eventos de Platform 
	y procesarlos posteriormente en Input.
*/
class InputListener : public SDL_Listener {
public:
	InputListener();
	~InputListener();
	virtual void notify(SDL_Event* evt);
	
	bool _keyUpEvent,
		 _keyDownEvent,
		 _mouseMoveEvent,
		 _mouseButtonEvent;
};