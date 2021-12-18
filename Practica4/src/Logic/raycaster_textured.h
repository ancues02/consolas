/*
Copyright (c) 2004-2019, Lode Vandevenne

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cmath>
#include <string>
#include <vector>
#include <iostream>

/*
g++ *.cpp -lSDL -O3 -W -Wall -ansi -pedantic
g++ *.cpp -lSDL
*/

#define screenWidth 1080
#define screenHeight 1920
#define texWidth 64
#define texHeight 64
#define mapWidth 64
#define mapHeight 64

class Rayos {
private:
    static uint32_t buffer[screenHeight][screenWidth];
public:
    static int mainRay(double posX, double posY, double ang, int worldMap[mapWidth][mapHeight])
    {
        //double posX = 22.0, posY = 11.5;  //x and y start position
        //double dirX = -1.0, dirY = 0.0; //initial direction vector
        double dirX = cos(ang);
        double dirY = sin(ang);
        double planeX = 0.0, planeY = 0.66; //the 2d raycaster version of camera plane

        double time = 0; //time of current frame
        double oldTime = 0; //time of previous frame

        std::vector<uint32_t> texture[8];
        for (int i = 0; i < 8; i++) texture[i].resize(texWidth * texHeight);

        //screen(screenWidth,screenHeight, 0, "Raycaster");

        int h = 1080;
        int w = 1920;

        for (int x = 0; x < w; x++)
        {
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
                if (worldMap[mapX][mapY] > 0) hit = 1;
            }

            //Calculate distance of perpendicular ray (Euclidean distance would give fisheye effect!)
            if (side == 0) perpWallDist = (sideDistX - deltaDistX);
            else          perpWallDist = (sideDistY - deltaDistY);

            //Calculate height of line to draw on screen
            int lineHeight = (int)(h / perpWallDist);

            int pitch = 100;

            //calculate lowest and highest pixel to fill in current stripe
            int drawStart = -lineHeight / 2 + h / 2 + pitch;
            if (drawStart < 0) drawStart = 0;
            int drawEnd = lineHeight / 2 + h / 2 + pitch;
            if (drawEnd >= h) drawEnd = h - 1;

            //texturing calculations
            int texNum = worldMap[mapX][mapY] - 1; //1 subtracted from it so that texture 0 can be used!

            //calculate value of wallX
            double wallX; //where exactly the wall was hit
            if (side == 0) wallX = posY + perpWallDist * rayDirY;
            else          wallX = posX + perpWallDist * rayDirX;
            wallX -= floor((wallX));
          
            // TODO: an integer-only bresenham or DDA like algorithm could make the texture coordinate stepping faster
            // How much to increase the texture coordinate per screen pixel
            double step = 1.0 * texHeight / lineHeight;
            // Starting texture coordinate
            double texPos = (drawStart - pitch - h / 2 + lineHeight / 2) * step;
            for (int y = drawStart; y < drawEnd; y++)
            {
                // Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
                int texY = (int)texPos & (texHeight - 1);
                texPos += step;
                uint32_t color = texture[texNum][texHeight * texY];
                //make color darker for y-sides: R, G and B byte each divided through two with a "shift" and an "and"
                if (side == 1) color = (color >> 1) & 8355711;
                buffer[y][x] = color;
            }
        }
        return 0;
    }
};