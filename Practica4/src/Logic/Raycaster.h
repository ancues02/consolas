#pragma once

class Map;

struct RaycastData {
	RaycastData(float x, float y, float d, bool v) : posX(x), posY(y), distance(d), vertical(v) {}

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