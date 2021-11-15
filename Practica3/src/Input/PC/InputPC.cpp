#ifdef PLATFORM_PC
#include "InputPC.h"

const uint8_t* Input::_keyboard = nullptr;
bool Input::_keyUpEvent = false;
bool Input::_keyDownEvent = false;
bool Input::_mouseMoveEvent = false;
bool Input::_mouseButtonEvent = false;

bool Input::_mouseState[3] = { false, false, false };
int Input::_mouseX = 0;
int Input::_mouseY = 0;

void Input::Init()
{
	_keyboard = SDL_GetKeyboardState(0);
	// faltaria hacer metodos
	// isKeyUp(key), para el raton y todo eso
	// pero me voy a dormir
}

void Input::Tick()
{
	//SDL_Event _event;
	/*
	Actualizar posicion del raton?
	*/
}

void Input::Release()
{
}

void Input::beNotified(SDL_Event* evt)
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

void Input::reset()
{
	_keyUpEvent = false;
	_keyDownEvent = false;
	_mouseMoveEvent = false;
	_mouseButtonEvent = false;

	for (int i = 0; i < 3; i++)
		_mouseState[i] = false;
}
#endif