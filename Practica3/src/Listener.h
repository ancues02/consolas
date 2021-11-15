#pragma once
class Listener {
protected:
	virtual void recieveMSG(void* msg) = 0;
};