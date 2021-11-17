#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include "Platform/Platform.h"
#include "Renderer/Renderer.h"
#include "Input/Input.h"

#include "Utils.h"
#include "Logic/Map.h"
#include <stdio.h>
#include <algorithm>

const unsigned int BASE_TILE_SIZE = 64;
const unsigned int PLAYER_SIZE = 5;
const float ZOOM_PER_SECOND = 1;
const float TILES_PER_SECOND = 3;
const float SCALE_MAX = 3.0f;
const float SCALE_MIN = 0.5f;
const float COLLISION_OFFSET = 0.2f;
float scale = 1.0f;

// Posicion del jugador
float posPlayerX, posPlayerY;

// Mapas
uint16_t numMaps = 0;//numero de mapas que hay
Map* maps = nullptr;//todos los mapas
int mapIndex = 0;//El indicide el mapa que estamos usando

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
	posPlayerX = maps[mapIndex].getPlayerSpawnPoint() % BASE_TILE_SIZE;
	posPlayerY = maps[mapIndex].getPlayerSpawnPoint() / BASE_TILE_SIZE;
}


void drawMap(const Map& map) {

	int finalTileSize = BASE_TILE_SIZE * scale;
	for (int j = 0; j < map.getSize(); ++j) {
		if (map.getTile(j) <= 63 && map.getTile(j) > 0) {
			Renderer::DrawImage(*Renderer::GetImage(2* map.getTile(j) -2), 
				((j % BASE_TILE_SIZE) - posPlayerX) * finalTileSize + Renderer::GetWidth() / 2,
				((j / BASE_TILE_SIZE) - posPlayerY) * finalTileSize + Renderer::GetHeight() / 2, 
				finalTileSize, 
				finalTileSize);
		}
	}

	Renderer::DrawRect((Renderer::GetWidth() / 2) - (PLAYER_SIZE / 2), 
		(Renderer::GetHeight() / 2) - (PLAYER_SIZE / 2),
		PLAYER_SIZE, 
		PLAYER_SIZE, 
		{ 255, 255, 0, 0 });
}

void draw() {
	drawMap(maps[mapIndex]);
}


void update(double deltaTime) {
	float posY = Input::GetHorizontalAxis();
	float posX = Input::GetVerticalAxis();
	float zoom = Input::GetZoom();
	
	scale += zoom * deltaTime * ZOOM_PER_SECOND;
	
	// Clamp
	scale = std::max(SCALE_MIN, scale);
	scale = std::min(scale, SCALE_MAX);
	


	float nextX = posPlayerX + posX * TILES_PER_SECOND * deltaTime;
	float nextY = posPlayerY + posY * TILES_PER_SECOND * deltaTime;
	//te mueves en horizontal solo si ni en tu actual Y, ni en la siguiente hacia arriba del tile
	// ni en la de abajo del tile hay un tile
	if (maps[mapIndex].isTransitable(nextX + (COLLISION_OFFSET * posX), posPlayerY )
		&& maps[mapIndex].isTransitable(nextX + (COLLISION_OFFSET * posX), posPlayerY + (COLLISION_OFFSET * posX))
		&& maps[mapIndex].isTransitable(nextX + (COLLISION_OFFSET * posX), posPlayerY + (COLLISION_OFFSET * -posX))) {
		posPlayerX = nextX;
	}
	//te mueves en vertical similar a horizontal pero comprobaciones en la x
	if (maps[mapIndex].isTransitable(posPlayerX , nextY + (COLLISION_OFFSET * posY)) &&
		maps[mapIndex].isTransitable(posPlayerX + (COLLISION_OFFSET * posY), nextY + (COLLISION_OFFSET * posY))
		&& maps[mapIndex].isTransitable(posPlayerX + (COLLISION_OFFSET * -posY), nextY + (COLLISION_OFFSET * posY))) {
		posPlayerY = nextY;
	}
}

void free() {
	delete[] maps;
}

int main(int argc, char* argv[])
{
	Platform::Init();
	Renderer::Init(false, 1280, 720);
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