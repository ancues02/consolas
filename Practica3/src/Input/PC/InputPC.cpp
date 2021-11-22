#ifdef PLATFORM_PC
#include "InputPC.h"
#include "../../Platform/Platform.h"

InputListener Input::_inputListener;
InputInfo Input::_frameInfo;
const uint8_t* Input::_keyboard = nullptr;
SDL_GameController* Input::_controller = nullptr;
int32_t Input::_currentController;

bool Input::Init()
{
    // Referencia al teclado para toda la ejecucion
    _keyboard = SDL_GetKeyboardState(0);

    // Ponemos a escuchar los eventos a nuestro listener
    Platform::addInputListener(&_inputListener);

    return true;
}

void Input::Tick()
{
    _frameInfo = _inputListener.getFrameInfo();

    if (_frameInfo._controllerConnected) {  // Último mando conectado
        if (!_controller) {
            _controller = SDL_GameControllerOpen(_frameInfo._controllerCId);
            _currentController = _frameInfo._controllerCId;
        }
    }
    if (_frameInfo._controllerDisconnected) { 
        if (_currentController == _frameInfo._controllerDId) {  // Si era el current lo cierra
            SDL_GameControllerClose(_controller);         
            _controller = nullptr;
            _currentController = -1;
            for (int i = 0; i < SDL_NumJoysticks(); ++i) {  // y por defecto coge el primero que encuentre
                _controller = SDL_GameControllerOpen(i);
                _currentController = i;
            }
                       
            //_currentController = SDL_getcontroller;
        }
    }
}

void Input::Release()
{
    if(_controller) SDL_GameControllerClose(_controller);
}

void Input::GetAxis(float& horizontal, float& vertical) {
    vertical = horizontal = 0;

    vertical -= _keyboard[SDL_SCANCODE_W];
    vertical += _keyboard[SDL_SCANCODE_S];

    horizontal -= _keyboard[SDL_GetScancodeFromKey(SDLK_a)];
    horizontal += _keyboard[SDL_GetScancodeFromKey(SDLK_d)];

    float vCont = 0;
    if (_controller) {
        float cAxis = (float)SDL_GameControllerGetAxis(_controller, SDL_CONTROLLER_AXIS_LEFTY);
        float maxAxis = SDL_JOYSTICK_AXIS_MAX;
        float tAxis = cAxis / maxAxis;
        vCont += tAxis;
    }

    float hCont = 0;
    if (_controller) {
        float cAxis = (float)SDL_GameControllerGetAxis(_controller, SDL_CONTROLLER_AXIS_LEFTX);
        float maxAxis = SDL_JOYSTICK_AXIS_MAX;
        float tAxis = cAxis / maxAxis;
        hCont += tAxis;
    }

    if (abs(hCont) < JOYSTICK_DEADZONE && abs(vCont) < JOYSTICK_DEADZONE) 
    {
        hCont = vCont = 0;
    }

    horizontal += hCont;
    vertical += vCont;

    clampf(vertical, -1, 1);
    clampf(horizontal, -1, 1);
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