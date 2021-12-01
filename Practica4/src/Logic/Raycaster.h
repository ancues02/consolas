#pragma once

const int NUMBER_OF_RAYS = 1920;

class Map;

struct RaycastData {
	bool hit;
	float posX, posY;
	float distance;
	bool vertical;
};

class Raycaster {
public:
	Raycaster();
	~Raycaster();
	void CastRays(float posX, float posY, float angleStart, float FOV, const Map& collisionData);
	RaycastData* getRays();
private:
	void CastRay(int index, float posX, float posY, float angle, const Map& collisionData);

	RaycastData* _raydata;
};