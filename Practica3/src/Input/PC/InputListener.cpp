#include "InputListener.h"
#include "InputPC.h"

InputListener::InputListener() : 
	_keyDownEvent(false), 
	_keyUpEvent(false), 
	_mouseMoveEvent(false), 
	_mouseButtonEvent(false)
{
}

InputListener::~InputListener()
{
}

void InputListener::notify(SDL_Event* evt)
{
	switch (evt->type) {
	case SDL_KEYDOWN:
		_keyDownEvent = true;
		break;
	case SDL_KEYUP:
		_keyUpEvent = true; //aqui hacer mas cosas y en los demas case
		break;
	case SDL_MOUSEMOTION:
		_mouseMoveEvent = true;
		break;
	case SDL_MOUSEBUTTONDOWN:
		_mouseButtonEvent = true;
		break;
	case SDL_MOUSEBUTTONUP:
		_mouseButtonEvent = true;
		break;
	}
}
