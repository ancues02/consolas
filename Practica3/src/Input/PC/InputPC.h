#pragma once
#include <queue>
#include "../../Utils.h"

#include <SDL_events.h>

class Input{
public:
	enum MouseState : uint8_t {
		LEFT,
		RIGHT,
		MID
	};

	static void Init();
	static void Tick();
	static void Release();

	// Metodo de listener no se si esto esta bien
	// porque se podria llamar desde main
	static void beNotified(SDL_Event* evt);

private:
	static const uint8_t* _keyboard;
	static bool _keyUpEvent,
				_keyDownEvent,
				_mouseMoveEvent,
				_mouseButtonEvent;

	static bool _mouseState[3];
	static int _mouseX;
	static int _mouseY;

	static void reset();
};
