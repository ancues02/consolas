#ifdef PLATFORM_PC
#include "PlatformPC.h"
#include <SDL.h>
#include <SDL_keycode.h>
#include <SDL_mouse.h>
#include <SDL_events.h>
#include <SDL_surface.h>
#include <iostream>
#include <string>
#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>

#include "../../Utils.h"

std::map<const char*, FILE*> Platform::_fileMap;

void Platform::Init()
{
	int e = SDL_Init(SDL_INIT_EVERYTHING);
	if (e > 0)
		throw "Algo fallo iniciando SDL";

}

bool Platform::Tick()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE)
			return false;	
		//notifyListeners(&event);
	}

	return true;
}

void Platform::Release()
{
	SDL_Quit();
}

FILE* Platform::OpenFile(const char *name, const char *flags)
{
	auto it = _fileMap.find(name);
	FILE* file;

	if (it == _fileMap.end()) {
		file = fopen(name, flags);
		//TODO: Error handling
		_fileMap.insert({ name, file });
	}
	else {
		file = it->second;
	}

	return file;
}

//TODO: Error handling 
void Platform::CloseFile(const char *name)
{
	auto file_it = _fileMap.find(name);
	if (file_it == _fileMap.end()) {
		//ERRROR
	}
	else {
		_fileMap.erase(file_it);
		std::fclose(file_it->second);
	}
}

bool Platform::IsBigEndian() {
	return SDL_BYTEORDER == SDL_BIG_ENDIAN;
}

#endif

void Platform::addInputListener(Listener* listener)
{
	_inputListeners.push_back(listener);
}

void Platform::removeInputListener(Listener* listener)
{
	for (auto it = _inputListeners.begin(); it != _inputListeners.end(); ++it) {
		if ((*it) == listener){
			_inputListeners.erase(it);
			break;
		}
	}

}

void Platform::notifyListeners(SDL_Event* evt)
{
	for (Listener* listener : _inputListeners) {
		listener->notify(evt);
	}
}
