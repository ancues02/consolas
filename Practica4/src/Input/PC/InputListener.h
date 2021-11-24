#pragma once

#include "../../Platform/PC/Listener.h"


// Estructura para recoger el input y poder resetearlo cada frame
struct InputInfo
{
	bool _keyUpEvent = false;
	bool _keyDownEvent = false;
	bool _mouseMoveEvent = false;
	bool _mouseButtonEvent = false;
	bool _controllerConnected = false;
	bool _controllerDisconnected = false;
	Sint32 _controllerCId = -1;
	Sint32 _controllerDId = -1;
};

/*
	Clase para escuchar los eventos de Platform 
	y procesarlos posteriormente en Input.
*/
class InputListener : public SDL_Listener {
public:
	InputListener();
	~InputListener();
	virtual void notify(SDL_Event* evt);
	InputInfo getFrameInfo();

private:
	InputInfo _inputInfo;
	void resetInfo();
};