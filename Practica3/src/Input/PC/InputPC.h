#pragma once
#include "../../Utils.h"
#include "InputListener.h"
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

	static float GetHorizontalAxis();
	static float GetVerticalAxis();
	static int GetZoom();

private:
	// Listener para escuchar los eventos de Platform
	static InputListener _inputListener;

	// Referencia al teclado para toda la ejecucion
	static const uint8_t* _keyboard;
};
