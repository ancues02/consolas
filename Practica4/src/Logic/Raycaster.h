#pragma once

//const int NUMBER_OF_RAYS = 1920;
#include "../Renderer/RenderCommand.h"

class Map;

struct RaycastData {
public:
	bool hit;
	float posX, posY;
	int mapX, mapY;
	float rayDirX, rayDirY;
	float distance;
	bool side;
};

class Raycaster {
public:
	Raycaster(int h, int w, int tileSize);
	~Raycaster();
	void CastRays(const float& posX, const float& posY, const float& angleStart, const float& FOV, const Map& collisionData);
	RaycastData* getRays();
private:
	void CastRay(const int& x, const float& posX, const float& posY, const float& ang, const Map& collisionData);
	RenderCommand _rC;
	RaycastData* _raydata;
	int _h, _w, _tileSize;
};