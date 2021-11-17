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

const int nameSize = 16;
const unsigned int baseTileSize = 64;
const unsigned int playerSize = 20;
float scale = 1.0f;

// Posicion del jugador
float posPlayerX, posPlayerY;

// Mapas
uint16_t numMaps = 0;
Map* maps = nullptr;
int mapIndex = 0;

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
	posPlayerX = maps[mapIndex].getPlayerSpawnPoint() % baseTileSize;
	posPlayerY = maps[mapIndex].getPlayerSpawnPoint() / baseTileSize;
}

float toRad(float deg) {
	return (deg * 3.14) / 180;
}

void drawMap(const Map& map) {

	int finalTileSize = baseTileSize * scale;
	for (int j = 0; j < map.getSize(); ++j) {
		if (map.getTile(j) <= 63 && map.getTile(j) > 0) {
			Renderer::DrawImage(*Renderer::GetImage(2* map.getTile(j) -2), 
				((j % baseTileSize) - posPlayerX) * finalTileSize + Renderer::GetWidth() / 2 - finalTileSize /2,
				((j / baseTileSize) - posPlayerY) * finalTileSize + Renderer::GetHeight() / 2 - finalTileSize/2, 
				finalTileSize, 
				finalTileSize);
		}
	}

	Renderer::DrawRect((Renderer::GetWidth() / 2) - (playerSize / 2), 
		(Renderer::GetHeight() / 2) - (playerSize / 2),
		playerSize, 
		playerSize, 
		{ 255, 255, 0, 0 });
}

void draw() {
	drawMap(maps[mapIndex]);
}

void handleInput() {
	float posY = Input::GetHorizontalAxis();
	float posX = Input::GetVerticalAxis();

	posX /=  50;
	posY /=  50;

	posPlayerX += posX ;
	posPlayerY += posY ;
}

void free() {
	delete[] maps;
}

int main(int argc, char* argv[])
{
	Platform::Init();
	Renderer::Init(false, 1920, 1080);
	Input::Init();

	Renderer::ReadImage("assets/walls.pak");

	lecturaMapa("assets/maps.pak");

	while (Platform::Tick())
	{
		Input::Tick();
		handleInput();
		Renderer::Clear({ 255, 0, 0, 0 });//limpiamos a color negro por defecto
		draw();
		Renderer::Present();
	}

	free();
	Renderer::Release();
	Platform::Release();

	return 0;
}