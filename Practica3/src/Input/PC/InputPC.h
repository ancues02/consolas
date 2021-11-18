#pragma once
#include "../../Utils.h"
#include "InputListener.h"

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

	// Informacion sobre el input recibido en el frame
	static InputInfo _frameInfo;

	// Referencia al teclado para toda la ejecucion
	static const uint8_t* _keyboard;

	// referencia al GamePad para toda la ejecución
	static SDL_GameController* _controller;
};
