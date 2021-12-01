#include "Raycaster.h"
#include "Map.h"

#include <math.h>
#include <stdio.h> 
#include <iostream>

Raycaster::Raycaster()
{
	_raydata = new RaycastData[NUMBER_OF_RAYS];
}

Raycaster::~Raycaster()
{
	delete[] _raydata;
}

void Raycaster::CastRays(float posX, float posY, float angleStart, float FOV, const Map& collisionData) {
	float angle = angleStart - FOV / 2.0f;
	float angIncr = FOV / (float)NUMBER_OF_RAYS;
	for (int i = 0; i < NUMBER_OF_RAYS; ++i) {
		CastRay(i, posX, posY, angleStart/*angle + i * angIncr*/, collisionData);
	}
}

RaycastData* Raycaster::getRays() {
	return _raydata;
}

void Raycaster::CastRay(int index, float posX, float posY, float angle, const Map& collisionData)
{
	float rayX = cos(angle);
	int xMult = rayX < 0 ? -1 : 1;
	float rayY = sin(angle);
	int yMult = rayY < 0 ? -1 : 1;
	//std::cout << "RAYX: " << rayX << " RAYY: " << rayY << "ANGLE: " << angle << "\n";

	float catetoSigTile = xMult;
	float catetoAux = rayY / rayX;
	float deltaDistX = sqrt(pow(catetoAux, 2) + pow(catetoSigTile, 2));

	catetoSigTile = yMult;
	catetoAux = rayX / rayY;
	float deltaDistY = sqrt(pow(catetoAux, 2) + pow(catetoSigTile, 2));

	float sideDistX = (1.0f - (posX - floorf(posX))) * deltaDistX;
	float sideDistY = (1.0f - (posY - floorf(posY))) * deltaDistY;

	float cposX = floor(posX);
	float cposY = floor(posY);

	float currDist = 0;
	float currDistX = sideDistX;
	float currDistY = sideDistY;

	bool vertical = true;

	while (cposX >= 0 && cposX < collisionData.getWidth() && 
		cposY >= 0 && cposY < collisionData.getHeight() &&
		collisionData.isTransitable(cposX, cposY))
	{
		if (abs(currDistX) < abs(currDistY)) {
			currDist += xMult * currDistX;
			currDistY -= currDistX;
			currDistX = deltaDistX;
			cposX += xMult;
			vertical = true;
		}
		else {
			currDist += yMult * currDistY;
			currDistX -= currDistY;
			currDistY = deltaDistY;
			cposY += yMult;
			vertical = false;
		}
	}

	if (vertical) {
		float catX = cposX - posX;
		float incrY = sqrt(pow(currDist, 2) - pow(catX, 2));
		cposY = posY + incrY;
	}
	else {
		float catY = cposY - posY;
		float incrX = sqrt(pow(currDist, 2) - pow(catY, 2));
		cposX = posX + incrX;
	}

	_raydata[index].hit = cposX >= 0 && cposX < collisionData.getWidth() && cposY >= 0 && cposY < collisionData.getHeight();
	_raydata[index].posX = cposX;
	_raydata[index].posY = cposY;
	_raydata[index].distance = currDist;
	_raydata[index].vertical = vertical;
}
