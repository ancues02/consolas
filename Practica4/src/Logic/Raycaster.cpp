#include "Raycaster.h"
#include "Map.h"

#include <math.h>
#include <stdio.h> 
#include <iostream>
#include "../Renderer/RenderThread.h"


Raycaster::Raycaster(int hp, int wp, int tileSize) : _h(hp), _w(wp), _tileSize(tileSize)
{
	_raydata = new RaycastData[_w];
    _rayLines = new RayLines[_w];
    _rC = new RenderCommand();
    _rC->rayLinesInfo.rl = _rayLines;
    _rC->tipo = RenderCommandType::RAY_LINES;
    //_rC->drawTextureLineInfo.x2 = 1;

}

Raycaster::~Raycaster()
{
	delete[] _raydata;
}

void Raycaster::CastRays(const float& posX, const float& posY, const float& angleStart, const float& FOV, const Map& collisionData) {
	
	for (int x= 0; x < _w; ++x) {
        CastRay(x, posX, posY, angleStart, collisionData); 
	}
    RenderThread::addCommand(*_rC);
}

RaycastData* Raycaster::getRays() {
	return _raydata;
}

void Raycaster::CastRay(const int& x, const float& posX, const float& posY, const float& ang, const Map& collisionData) {
    float dirX = cos(ang);
    float dirY = sin(ang);

    float planeX = -dirY , planeY = dirX ; //the 2d raycaster version of camera plane

    //calculate ray position and direction
    float cameraX = 2 * x / (float)_w - 1; //x-coordinate in camera space
    _raydata[x].rayDirX = dirX + planeX * cameraX;
    _raydata[x].rayDirY = dirY + planeY * cameraX;

    //which box of the map we're in
    _raydata[x].mapX = int(posX);
    _raydata[x].mapY = int(posY);

    //length of ray from current position to next x or y-side
    float sideDistX;
    float sideDistY;

    //length of ray from one x or y-side to next x or y-side
    float deltaDistX = std::abs(1 / _raydata[x].rayDirX);
    float deltaDistY = std::abs(1 / _raydata[x].rayDirY);

    //what direction to step in x or y-direction (either +1 or -1)
    int stepX;
    int stepY;

    _raydata[x].hit = false; //was there a wall hit?

    //calculate step and initial sideDist
    if (_raydata[x].rayDirX < 0)
    {
        stepX = -1;
        sideDistX = (posX - _raydata[x].mapX) * deltaDistX;
    }
    else
    {
        stepX = 1;
        sideDistX = (_raydata[x].mapX + 1.0 - posX) * deltaDistX;
    }
    if (_raydata[x].rayDirY < 0)
    {
        stepY = -1;
        sideDistY = (posY - _raydata[x].mapY) * deltaDistY;
    }
    else
    {
        stepY = 1;
        sideDistY = (_raydata[x].mapY + 1.0 - posY) * deltaDistY;
    }
    //perform DDA
    while (!_raydata[x].hit)
    {
        //jump to next map square, either in x-direction, or in y-direction
        if (sideDistX < sideDistY)
        {
            sideDistX += deltaDistX;
            _raydata[x].mapX += stepX;
            _raydata[x].side = false;
        }
        else
        {
            sideDistY += deltaDistY;
            _raydata[x].mapY += stepY;
            _raydata[x].side = true;
        }
        //Check if ray has hit a wall
        _raydata[x].hit = !collisionData.isTransitable(_raydata[x].mapX, _raydata[x].mapY);
    }

    //Calculate distance of perpendicular ray (Euclidean distance would give fisheye effect!)
   
    _raydata[x].distance = _raydata[x].side ? sideDistY - deltaDistY : sideDistX - deltaDistX;

    _raydata[x].posX = posX;
    _raydata[x].posY = posY;  

    int lineHeight = (int)(_h / _raydata[x].distance * tan(M_PI*0.25f));

    //calculate lowest and highest pixel to fill in current stripe
    int drawStart = -lineHeight / 2 + _h / 2;
    int drawEnd = lineHeight / 2 + _h / 2;

    int j = _raydata[x].mapX + (_raydata[x].mapY * collisionData.getWidth());

    //calculate value of wallX //where exactly the wall was hit
    float wallX = _raydata[x].side ? _raydata[x].posX + _raydata[x].distance * _raydata[x].rayDirX : _raydata[x].posY + _raydata[x].distance * _raydata[x].rayDirY;

    wallX -= floor(wallX);

    //x coordinate on the texture
    int texX = int(wallX * _tileSize);
    if ((_raydata[x].side == 0 && _raydata[x].rayDirX < 0) || (_raydata[x].side == 1 && _raydata[x].rayDirY > 0))
        texX = _tileSize - texX - 1;
    //Renderer::DrawLine(i, drawStart, i, drawEnd, data[i].vertical ? Color({ 255, 0, 255, 0 }) : Color({ 255, 255, 0, 0 }));
   
    _rayLines[x].im = Renderer::GetImage(2 * collisionData.getTile(j) - (1 + _raydata[x].side));
    _rayLines[x].texX = texX;
    _rayLines[x].x1 = x;
    _rayLines[x].x2 = 1;
    _rayLines[x].y1 = drawStart;
    _rayLines[x].y2 = drawEnd - drawStart;

    /*_rC->drawTextureLineInfo.image = Renderer::GetImage(2 * collisionData.getTile(j) - (1 + _raydata[x].side));
    _rC->drawTextureLineInfo.texX = texX;
    _rC->drawTextureLineInfo.x1 = x;
    _rC->drawTextureLineInfo.y1 = drawStart;
    _rC->drawTextureLineInfo.y2 = drawEnd - drawStart;

    RenderThread::addCommand(*_rC);*/
}