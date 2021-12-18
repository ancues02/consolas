#pragma once

//const int NUMBER_OF_RAYS = 1920;

class Map;

struct RaycastData {
public:
	bool hit;
	float posX, posY;
	float mapX, mapY;
	float rayDirX, rayDirY;
	float distance;
	bool side;
};

class Raycaster {
public:
	Raycaster(int h, int w);
	~Raycaster();
	void CastRays(float posX, float posY, float angleStart, float FOV, const Map& collisionData);
	RaycastData* getRays();
private:
	void CastRay(int x, double posX, double posY, double ang, const Map& collisionData);

	RaycastData* _raydata;
	int h, w;
};