#pragma once
#include <queue>
#include "../../Utils.h"
#include "../../Listener.h"

class Input : public Listener {
public:
	static void Init();
	static void ParseInput();
	//static std::queue<EventType*> getInput();
	static void Release();
private:
	//static std::queue<EventType*> _events;

};
