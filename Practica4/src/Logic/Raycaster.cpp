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
    //calculate ray position and direction
    double cameraX = 2 * index / (double)NUMBER_OF_RAYS - 1; //x-coordinate in camera space
    double dirX = cos(angle);
    double dirY = sin(angle);
    double rayDirX = dirX + 0 * cameraX;
    double rayDirY = dirY + 0.66 * cameraX;
    //which box of the map we're in
    int mapX = int(posX);
    int mapY = int(posY);

    //length of ray from current position to next x or y-side
    double sideDistX;
    double sideDistY;

    //length of ray from one x or y-side to next x or y-side
    //these are derived as:
    //deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX))
    //deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY))
    //which can be simplified to abs(|rayDir| / rayDirX) and abs(|rayDir| / rayDirY)
    //where |rayDir| is the length of the vector (rayDirX, rayDirY). Its length,
    //unlike (dirX, dirY) is not 1, however this does not matter, only the
    //ratio between deltaDistX and deltaDistY matters, due to the way the DDA
    //stepping further below works. So the values can be computed as below.
    // Division through zero is prevented, even though technically that's not
    // needed in C++ with IEEE 754 floating point values.
    double deltaDistX = (rayDirX == 0) ? 1e30 : std::abs(1 / rayDirX);
    double deltaDistY = (rayDirY == 0) ? 1e30 : std::abs(1 / rayDirY);

    double perpWallDist;

    //what direction to step in x or y-direction (either +1 or -1)
    int stepX;
    int stepY;

    int hit = 0; //was there a wall hit?
    int side; //was a NS or a EW wall hit?
    //calculate step and initial sideDist
    if(rayDirX < 0)
    {
        stepX = -1;
        sideDistX = (posX - mapX) * deltaDistX;
    }
    else
    {
        stepX = 1;
        sideDistX = (mapX + 1.0 - posX) * deltaDistX;
    }
    if(rayDirY < 0)
    {
        stepY = -1;
        sideDistY = (posY - mapY) * deltaDistY;
    }
    else
    {
        stepY = 1;
        sideDistY = (mapY + 1.0 - posY) * deltaDistY;
    }
    //perform DDA
    while(collisionData.isTransitable(mapX, mapY))
    {
        //jump to next map square, either in x-direction, or in y-direction
        if(sideDistX < sideDistY)
        {
            sideDistX += deltaDistX;
            mapX += stepX;
            side = 0;
        }
        else
        {
            sideDistY += deltaDistY;
            mapY += stepY;
            side = 1;
        }
    }

    _raydata[index].hit = collisionData.isTransitable(mapX, mapY);
    _raydata[index].posX = mapX;
    _raydata[index].posY = mapY;
    _raydata[index].distance = 0;
    _raydata[index].vertical = side > 1;
}
//
//void Raycaster::CastRay(int index, float posX, float posY, float angle, const Map& collisionData)
//{
//	float rayX = cos(angle);
//	int xMult = rayX < 0 ? -1 : 1;
//	float rayY = sin(angle);
//	int yMult = rayY < 0 ? -1 : 1;
//	//std::cout << "RAYX: " << rayX << " RAYY: " << rayY << "ANGLE: " << angle << "\n";
//
//	float catetoSigTile = xMult;
//	float catetoAux = rayY / rayX;
//	float deltaDistX = sqrt(pow(catetoAux, 2) + pow(catetoSigTile, 2));
//
//	catetoSigTile = yMult;
//	catetoAux = rayX / rayY;
//	float deltaDistY = sqrt(pow(catetoAux, 2) + pow(catetoSigTile, 2));
//
//	float sideDistX = (1.0f - (posX - floorf(posX))) * deltaDistX;
//	float sideDistY = (1.0f - (posY - floorf(posY))) * deltaDistY;
//
//	float cposX = floor(posX);
//	float cposY = floor(posY);
//
//	float currDist = 0;
//	float currDistX = sideDistX;
//	float currDistY = sideDistY;
//
//	bool vertical = true;
//
//	while (cposX >= 0 && cposX < collisionData.getWidth() && 
//		cposY >= 0 && cposY < collisionData.getHeight() &&
//		collisionData.isTransitable(cposX, cposY))
//	{
//		if (abs(currDistX) < abs(currDistY)) {
//			currDist += xMult * currDistX;
//			currDistY -= currDistX;
//			currDistX = deltaDistX;
//			cposX += xMult;
//			vertical = true;
//		}
//		else {
//			currDist += yMult * currDistY;
//			currDistX -= currDistY;
//			currDistY = deltaDistY;
//			cposY += yMult;
//			vertical = false;
//		}
//	}
//
//	if (vertical) {
//		float catX = cposX - posX;
//		float incrY = sqrt(pow(currDist, 2) - pow(catX, 2));
//		cposY = posY + incrY;
//	}
//	else {
//		float catY = cposY - posY;
//		float incrX = sqrt(pow(currDist, 2) - pow(catY, 2));
//		cposX = posX + incrX;
//	}
//
//	_raydata[index].hit = cposX >= 0 && cposX < collisionData.getWidth() && cposY >= 0 && cposY < collisionData.getHeight();
//	_raydata[index].posX = cposX;
//	_raydata[index].posY = cposY;
//	_raydata[index].distance = currDist;
//	_raydata[index].vertical = vertical;
//}
