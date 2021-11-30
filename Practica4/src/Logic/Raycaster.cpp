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
	float sideDistY = (1.0f - posY) * deltaDistY;

	float cposX = posX;
	float cposY = posY;

	float currDist = 0;
	float currDistX = sideDistX;
	float currDistY = sideDistY;

	bool vertical = true;

	while (cposX < gameMap->getWidth() && cposY < gameMap->getHeight() &&
		gameMap->isTransitable(cposX, cposY)) 
	{
		if (currDistX < currDistY) {
			currDist += currDistX;
			currDistY -= currDistX;
			currDistX = deltaDistX;
			cposX++;
			vertical = true;
		}
		else {
			currDist += currDistY;
			currDistX -= currDistY;
			currDistY = deltaDistY;
			cposY++;
			vertical = false;
		}
	}
	// QUEDA PONER OTRA COSA AQUÍ, QUE SE QUEDA CORTO DE UN EJE

	if (cposX >= gameMap->getWidth() || cposY >= gameMap->getHeight())
		cposX = cposY = -1;

	RaycastData ray(cposX, cposY, currDist, vertical);

	return ray;
}
