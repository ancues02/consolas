#include "Raycaster.h"
#include "Map.h"

#include <math.h>

Raycaster::Raycaster() : gameMap(nullptr)
{
}

void Raycaster::SetMap(Map* collisionMap)
{
	gameMap = collisionMap;
}

RaycastData Raycaster::CastRay(float posX, float posY, float angle)
{
	float rayX = cos(angle);
	float rayY = sin(angle);

	float catetoSigTile = 1;
	float catetoAux = rayX / rayY;
	float deltaDistY = sqrt(pow(catetoAux, 2) + pow(catetoSigTile, 2));

	catetoAux = rayY / rayX;
	float deltaDistX = sqrt(pow(catetoAux, 2) + pow(catetoSigTile, 2));

	float sideDistX = (1.0f - posX) * deltaDistX;
	float sideDistX = (1.0f - posY) * deltaDistY;

	return RaycastData();
}
