#pragma once
#include <map>
#include <vector>
#include "../../Listener.h"

class SDL_Event;

class Platform {
public:
	static void Init();
	static bool Tick();
	static void Release();
	static FILE* OpenFile(const char* name, const char* flags = "r");
	static void CloseFile(const char* name);
	static bool IsBigEndian();
private:
	static std::map<const char*, FILE*> _fileMap;
	static std::vector<Listener*> _inputListeners;

	static void addInputListener(Listener* listener);
	static void removeInputListener(Listener* listener);
	static void notifyListeners(SDL_Event* evt);
};
