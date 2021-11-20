#ifdef PLATFORM_PC

#include "InputListener.h"
#include "InputPC.h"
#include <SDL_events.h>

InputListener::InputListener() : 
	_inputInfo()
{
}

InputListener::~InputListener()
{
}

void InputListener::notify(SDL_Event* evt)
{
	switch (evt->type) {
	case SDL_KEYDOWN:
		_inputInfo._keyDownEvent = true;
		break;
	case SDL_KEYUP:
		_inputInfo._keyUpEvent = true; //aqui hacer mas cosas y en los demas case
		break;
	case SDL_MOUSEMOTION:
		_inputInfo._mouseMoveEvent = true;
		break;
	case SDL_MOUSEBUTTONDOWN:
		_inputInfo._mouseButtonEvent = true;
		break;
	case SDL_MOUSEBUTTONUP:
		_inputInfo._mouseButtonEvent = true;
		break;
	case SDL_JOYDEVICEADDED:
		_inputInfo._controllerConnected = true;
		_inputInfo._controllerCId = evt->cdevice.which;
		break;
	case SDL_JOYDEVICEREMOVED:
		_inputInfo._controllerDisconnected = true;
		_inputInfo._controllerDId = evt->cdevice.which;
		break;
	}
}

InputInfo InputListener::getFrameInfo()
{
	InputInfo frameInfo = _inputInfo;
	// TODO: esto deberia de resetearse pero va peor
	resetInfo();
	return frameInfo;
}

void InputListener::resetInfo()
{
	_inputInfo._keyDownEvent = false;
	_inputInfo._keyUpEvent = false;
	_inputInfo._mouseButtonEvent = false;
	_inputInfo._mouseMoveEvent = false;
	_inputInfo._controllerConnected = false;
	_inputInfo._controllerDisconnected = false;
	_inputInfo._controllerCId = -1;
	_inputInfo._controllerDId = -1;
}

#endif // PLATFORM_PC