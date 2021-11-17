#ifdef PLATFORM_PC
#include "InputPC.h"
#include "../../Platform/Platform.h"

InputListener Input::_inputListener;
InputInfo Input::_frameInfo;
const uint8_t* Input::_keyboard = nullptr;

void Input::Init()
{
	// Referencia al teclado para toda la ejecucion
	_keyboard = SDL_GetKeyboardState(0);
	
	// Ponemos a escuchar los eventos a nuestro listener
	Platform::addInputListener(&_inputListener);
}

void Input::Tick()
{
	_frameInfo = _inputListener.getFrameInfo();
}

void Input::Release()
{
}

float Input::GetVerticalAxis()
{
	int ret = 0;
	if (_frameInfo._keyDownEvent) {
		if (_keyboard[SDL_SCANCODE_W])
			ret--;
		if (_keyboard[SDL_SCANCODE_S])
			ret++;
	}
	return ret;
}

float Input::GetHorizontalAxis()
{
	int ret = 0;
	if (_frameInfo._keyDownEvent) {
		if (_keyboard[SDL_GetScancodeFromKey(SDLK_a)])
			ret--;
		if (_keyboard[SDL_GetScancodeFromKey(SDLK_d)])
			ret++;
	}
	return ret;
}

int Input::GetZoom()
{
	int ret = 0;
	if (_frameInfo._keyDownEvent) {
		if (_keyboard[SDL_GetScancodeFromKey(SDLK_SPACE)])
			ret--;
		if (_keyboard[SDL_GetScancodeFromKey(SDLK_RETURN)])
			ret++;
	}
	return ret;
}

#endif