#pragma once
#include <map>
#include <vector>
#include <chrono>

#include "Listener.h"

class Platform {
public:
	static bool Init();
	static bool Tick();
	static void Release();

	//P3
	static FILE* OpenFile(const char* name, const char* flags = "rb");
	static void CloseFile(const char* name);
	static bool IsBigEndian();
	static void addInputListener(SDL_Listener* listener);
	static void removeInputListener(SDL_Listener* listener);
	static double getDeltaTime();

private:
	//P3
	static std::map<const char*, FILE*> _fileMap;
	static std::vector<SDL_Listener*> _inputListeners;
	
	static double deltaTime;
	static std::chrono::high_resolution_clock::time_point lastFrameMilli;
	static std::chrono::high_resolution_clock::time_point currentFrameMilli;

	static void notifyListeners(SDL_Event* evt);
};
