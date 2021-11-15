#pragma once
#include <map>
class string;
class SDL_Surface;

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
};
