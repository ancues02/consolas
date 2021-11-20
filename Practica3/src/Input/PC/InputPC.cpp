#ifdef PLATFORM_PC
#include "InputPC.h"
#include "../../Platform/Platform.h"

InputListener Input::_inputListener;
InputInfo Input::_frameInfo;
const uint8_t* Input::_keyboard = nullptr;
SDL_GameController* Input::_controller = nullptr;

bool Input::Init()
{
	// Referencia al teclado para toda la ejecucion
	_keyboard = SDL_GetKeyboardState(0);

	// Primer controller encontrado
	if (SDL_NumJoysticks() > 1) {
		_controller = SDL_GameControllerOpen(0);
	}
	
	// Ponemos a escuchar los eventos a nuestro listener
	Platform::addInputListener(&_inputListener);

	return true;
}

void Input::Tick()
{
	_frameInfo = _inputListener.getFrameInfo();

	if (_frameInfo._controllerConnected) {
		_controller = SDL_GameControllerOpen(0);
	}
	if (_frameInfo._controllerDisconnected) {
		if (SDL_NumJoysticks() > 1) 
			_controller = SDL_GameControllerOpen(0);
		else 
			_controller = nullptr;
	}
}

void Input::Release()
{
	delete _keyboard;
	delete _controller;
}

float Input::GetVerticalAxis()
{
	float ret = 0;

	ret -= _keyboard[SDL_SCANCODE_W];
	ret += _keyboard[SDL_SCANCODE_S];

	if (_controller) {
		float cAxis = (float)SDL_GameControllerGetAxis(_controller, SDL_CONTROLLER_AXIS_LEFTY);
		float maxAxis = SDL_JOYSTICK_AXIS_MAX;
		float tAxis = cAxis / maxAxis;
		if (abs(tAxis) > JOYSTICK_DEADZONE) 
			ret += tAxis;
	}
	clampf(ret, -1, 1);
	return ret;
}

float Input::GetHorizontalAxis()
{
	float ret = 0;

	ret -= _keyboard[SDL_GetScancodeFromKey(SDLK_a)];
	ret += _keyboard[SDL_GetScancodeFromKey(SDLK_d)];

	if (_controller) {
		float cAxis = (float)SDL_GameControllerGetAxis(_controller, SDL_CONTROLLER_AXIS_LEFTX);
		float maxAxis = SDL_JOYSTICK_AXIS_MAX;
		float tAxis = cAxis / maxAxis;
		if (abs(tAxis) > JOYSTICK_DEADZONE)
			ret += tAxis;
	}
	clampf(ret, -1, 1);
	return ret;
}

int Input::GetZoom()
{
	float ret = 0;

	ret -= _keyboard[SDL_GetScancodeFromKey(SDLK_SPACE)];
	ret += _keyboard[SDL_GetScancodeFromKey(SDLK_RETURN)];

	if (_controller) {
		ret += SDL_GameControllerGetButton(_controller, SDL_CONTROLLER_BUTTON_A);
		ret -= SDL_GameControllerGetButton(_controller, SDL_CONTROLLER_BUTTON_B);
	}
	clampf(ret, -1, 1);
	return ret;
}

#endif