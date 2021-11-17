#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include "Platform/Platform.h"
#include "Renderer/Renderer.h"
#include "Input/Input.h"

#include "Utils.h"
#include "Logic/Map.h"
#include "Logic/Player.h"
#include <stdio.h>
#include <algorithm>

const unsigned int BASE_TILE_SIZE = 64;
const float ZOOM_PER_SECOND = 1;
const float TILES_PER_SECOND = 3;
const float SCALE_MAX = 3.0f;
const float SCALE_MIN = 0.5f;
const float COLLISION_OFFSET = 0.2f;
float scale = 1.0f;

// Mapas
uint16_t numMaps = 0;	//numero de mapas que hay
Map* maps = nullptr;	//todos los mapas
int mapIndex = 0;		//El indicide el mapa que estamos usando

//Jugador
Player* playin;

void lecturaMapa(const char* fileName) {
	bool bigEndian = Platform::IsBigEndian();

	FILE* file = Platform::OpenFile(fileName, "rb");
	if (file == nullptr) return; // Ha fallado la carga

	// Numero de mapas
	if (fread(&numMaps, 2, 1, file) == 0) return;
	if (bigEndian)
		numMaps = FLIPENDIAN_16((int)numMaps);

	maps = new Map[numMaps];
	bool loaded = true;
	int i = 0;
	while (i < numMaps && !feof(file) && loaded) {
		loaded = maps[i].load(file, Platform::IsBigEndian());
		i++;
	}

	// TODO: esto en otro lado
	int sp = maps[mapIndex].getPlayerSpawnPoint();
	int dir = maps[mapIndex].getPlayerOrientation() % 19;
	playin = new Player((sp % BASE_TILE_SIZE) + 0.5f, (sp / BASE_TILE_SIZE) + 0.5f, DEG_RAD(((dir * 90) - 90)));
}


void drawMap(const Map& map) {
	int finalTileSize = BASE_TILE_SIZE * scale;
	for (int j = 0; j < map.getSize(); ++j) {
		if (map.getTile(j) <= 63 && map.getTile(j) > 0) {
			Renderer::DrawImage(*Renderer::GetImage(2* map.getTile(j) -2), 
				((j % BASE_TILE_SIZE) - playin->getPosX()) * finalTileSize + Renderer::GetWidth() / 2,
				((j / BASE_TILE_SIZE) - playin->getPosY()) * finalTileSize + Renderer::GetHeight() / 2, 
				finalTileSize, 
				finalTileSize);
		}
	}
}

void drawPlayer(const Player* Player) {
	Renderer::DrawRect((Renderer::GetWidth() / 2) - (PLAYER_SIZE / 2),
		(Renderer::GetHeight() / 2) - (PLAYER_SIZE / 2),
		PLAYER_SIZE,
		PLAYER_SIZE,
		{ 255, 255, 0, 0 });

	int contactX = (Renderer::GetWidth() / 2) + (cos(playin->getAngle()) * PLAYER_BAR_SIZE); 
	int contactY = (Renderer::GetHeight() / 2) + (sin(playin->getAngle()) * PLAYER_BAR_SIZE);
	Renderer::DrawLine(Renderer::GetWidth() / 2, Renderer::GetHeight() / 2, contactX, contactY, { 255, 255, 255, 255 });
}

void draw() {
	drawMap(maps[mapIndex]);
	drawPlayer(playin);
}


void update(double deltaTime) {
	float posX = Input::GetHorizontalAxis();
	float posY = Input::GetVerticalAxis();
	float zoom = Input::GetZoom();

	playin->calculateAngle(posX, posY);

	scale += zoom * deltaTime * ZOOM_PER_SECOND;
	
	// Clamp
	scale = std::max(SCALE_MIN, scale);
	scale = std::min(scale, SCALE_MAX);

	float nextX = playin->getPosX() + posX * TILES_PER_SECOND * deltaTime;
	float nextY = playin->getPosY() + posY * TILES_PER_SECOND * deltaTime;
	//te mueves en horizontal solo si ni en tu actual Y, ni en la siguiente hacia arriba del tile
	// ni en la de abajo del tile hay un tile
	if (maps[mapIndex].isTransitable(nextX + (COLLISION_OFFSET * posX), playin->getPosY())
		&& maps[mapIndex].isTransitable(nextX + (COLLISION_OFFSET * posX), playin->getPosY() + (COLLISION_OFFSET * posX))
		&& maps[mapIndex].isTransitable(nextX + (COLLISION_OFFSET * posX), playin->getPosY() + (COLLISION_OFFSET * -posX))) {
		playin->setPosX(nextX);
	}
	//te mueves en vertical similar a horizontal pero comprobaciones en la x
	if (maps[mapIndex].isTransitable(playin->getPosX(), nextY + (COLLISION_OFFSET * posY)) &&
		maps[mapIndex].isTransitable(playin->getPosX() + (COLLISION_OFFSET * posY), nextY + (COLLISION_OFFSET * posY))
		&& maps[mapIndex].isTransitable(playin->getPosX() + (COLLISION_OFFSET * -posY), nextY + (COLLISION_OFFSET * posY))) {
		playin->setPosY(nextY);
	}
}

void free() {
	delete[] maps;
	delete playin;
}

int main(int argc, char* argv[])
{
	Platform::Init();
	Renderer::Init(false, 1920, 1080);
	Input::Init();

	Renderer::ReadImage("assets/walls.pak");

	lecturaMapa("assets/maps.pak");

	std::chrono::high_resolution_clock::time_point lastFrameTime, actualFrameTime;
	double deltaTime;
	lastFrameTime = std::chrono::high_resolution_clock::now();
	while (Platform::Tick())
	{
		actualFrameTime = std::chrono::high_resolution_clock::now();
		deltaTime = (std::chrono::duration_cast<std::chrono::milliseconds>(actualFrameTime - lastFrameTime).count())/1000.0;
		lastFrameTime = actualFrameTime;

		Input::Tick();
		update(deltaTime);
		Renderer::Clear({ 255, 0, 0, 0 });//limpiamos a color negro por defecto
		draw();
		Renderer::Present();
	}
	

	free();
	Renderer::Release();
	Platform::Release();

	return 0;
}