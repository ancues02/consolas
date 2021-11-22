#ifdef PLATFORM_PC
#include "PlatformPC.h"
#include <SDL.h>
#include <SDL_keycode.h>
#include <SDL_mouse.h>
#include <SDL_surface.h>
#include <iostream>
#include <string>
#include <stdio.h>

#include "../../Utils.h"

std::map<const char*, FILE*> Platform::_fileMap;
std::vector<SDL_Listener*> Platform::_inputListeners;
double Platform::deltaTime;
std::chrono::high_resolution_clock::time_point Platform::lastFrameMilli = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point Platform::currentFrameMilli = std::chrono::high_resolution_clock::now();

bool Platform::Init()
{
	int e = SDL_Init(SDL_INIT_EVERYTHING);
	if (e > 0) {
		std::cerr << "Algo fallo iniciando SDL" << std::endl;
		return false;
	}

	return true;
}

bool Platform::Tick()
{
	lastFrameMilli = currentFrameMilli;
	currentFrameMilli = std::chrono::high_resolution_clock::now();
	deltaTime = (std::chrono::duration_cast<std::chrono::milliseconds>(currentFrameMilli - lastFrameMilli).count()) / 1000.0;

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE)
			return false;	
		notifyListeners(&event);
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
		if (!file)
			return nullptr;
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
		std::cerr << "Error, no existe el mapa " << name << std::endl;
	}
	else {
		_fileMap.erase(file_it);
		std::fclose(file_it->second);
	}
}

bool Platform::IsBigEndian() {
	return SDL_BYTEORDER == SDL_BIG_ENDIAN;
}


void Platform::addInputListener(SDL_Listener* listener)
{
	_inputListeners.push_back(listener);
}

void Platform::removeInputListener(SDL_Listener* listener)
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
	for (SDL_Listener* listener : _inputListeners) {
		listener->notify(evt);
	}
}

double Platform::getDeltaTime()
{
	return deltaTime;
}

#endif