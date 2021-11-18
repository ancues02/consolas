#ifdef PLATFORM_PC
#include "InputPC.h"
#include "../../Platform/Platform.h"

InputListener Input::_inputListener;
InputInfo Input::_frameInfo;
const uint8_t* Input::_keyboard = nullptr;
SDL_GameController* Input::_controller = nullptr;

void Input::Init()
{
	// Referencia al teclado para toda la ejecucion
	_keyboard = SDL_GetKeyboardState(0);

	// Primer controller encontrado
	if (SDL_NumJoysticks() > 1) {
		_controller = SDL_GameControllerOpen(0);
	}
	
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
	if (_frameInfo._joystickAxisEvent) {
		ret += (SDL_GameControllerGetAxis(_controller, SDL_CONTROLLER_AXIS_LEFTX) / (float)SDL_JOYSTICK_AXIS_MAX);
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
	if (_frameInfo._joystickAxisEvent) {
		ret += (SDL_GameControllerGetAxis(_controller, SDL_CONTROLLER_AXIS_LEFTY) / (float)SDL_JOYSTICK_AXIS_MAX);
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
	if (_frameInfo._joystickAxisEvent) {
		ret += SDL_GameControllerGetButton(_controller, SDL_CONTROLLER_BUTTON_A);
		ret -= SDL_GameControllerGetButton(_controller, SDL_CONTROLLER_BUTTON_B);
	}
	return ret;
}

#endif