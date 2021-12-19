#include "Raycaster.h"
#include "Map.h"

#include <math.h>
#include <stdio.h> 
#include <iostream>


Raycaster::Raycaster(int hp, int wp) : h(hp), w(wp)
{
	_raydata = new RaycastData[w];
}

Raycaster::~Raycaster()
{
	delete[] _raydata;
}

void Raycaster::CastRays(float posX, float posY, float angleStart, float FOV, const Map& collisionData) {
	/*float angle = angleStart - FOV / 2.0f;
	float angIncr = FOV / (float)NUMBER_OF_RAYS;*/
	for (int i = 0; i < w; ++i) {
		//CastRay(i, posX, posY, angleStart/*angle + i * angIncr*/, collisionData);
        CastRay(i, posX, posY, angleStart, collisionData);
	}
    //mainRay(posX, posY, angle, collisionData);
}

RaycastData* Raycaster::getRays() {
	return _raydata;
}

void Raycaster::CastRay(int x, double posX, double posY, double ang, const Map& collisionData) {
    double dirX = cos(ang);
    double dirY = sin(ang);

    double planeX = -dirY * 0.66, planeY = dirX * 0.66; //the 2d raycaster version of camera plane

    double time = 0; //time of current frame
    double oldTime = 0; //time of previous frame

    //calculate ray position and direction
    double cameraX = 2 * x / (double)w - 1; //x-coordinate in camera space
    double rayDirX = dirX + planeX * cameraX;
    double rayDirY = dirY + planeY * cameraX;

    //which box of the map we're in
    int mapX = int(posX);
    int mapY = int(posY);

    //length of ray from current position to next x or y-side
    double sideDistX;
    double sideDistY;

    //length of ray from one x or y-side to next x or y-side
    double deltaDistX = (rayDirX == 0) ? 1e30 : std::abs(1 / rayDirX);
    double deltaDistY = (rayDirY == 0) ? 1e30 : std::abs(1 / rayDirY);
    double perpWallDist;

    //what direction to step in x or y-direction (either +1 or -1)
    int stepX;
    int stepY;

    int hit = 0; //was there a wall hit?
    int side; //was a NS or a EW wall hit?

    //calculate step and initial sideDist
    if (rayDirX < 0)
    {
        stepX = -1;
        sideDistX = (posX - mapX) * deltaDistX;
    }
    else
    {
        stepX = 1;
        sideDistX = (mapX + 1.0 - posX) * deltaDistX;
    }
    if (rayDirY < 0)
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
    while (hit == 0)
    {
        //jump to next map square, either in x-direction, or in y-direction
        if (sideDistX < sideDistY)
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
        //Check if ray has hit a wall
        if (!collisionData.isTransitable(mapX, mapY)) hit = 1;
    }

    //Calculate distance of perpendicular ray (Euclidean distance would give fisheye effect!)
    if (side == 0) perpWallDist = (sideDistX - deltaDistX);
    else          perpWallDist = (sideDistY - deltaDistY);

    _raydata[x].hit = hit;
    _raydata[x].posX = posX;
    _raydata[x].posY = posY;  
    _raydata[x].mapX = mapX;
    _raydata[x].mapY = mapY;
    _raydata[x].rayDirX = rayDirX;
    _raydata[x].rayDirY = rayDirY;
    _raydata[x].distance = perpWallDist;
    _raydata[x].side = side;
}