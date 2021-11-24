#pragma once

class Map;

struct RaycastData {
	float posX, posY;
	float distance;
	bool vertical;
};

class Raycaster {
public:
	Raycaster();

	void SetMap(Map* collisionMap);
	RaycastData CastRay(float posX, float posY, float angle);
private:
	Map* gameMap = nullptr;
};