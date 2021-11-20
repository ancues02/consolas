#include <iostream>
#include <fstream>
#include <chrono>
#include <stdio.h>
#include <algorithm>

#include "Platform/Platform.h"
#include "Renderer/Renderer.h"
#include "Input/Input.h"
#include "Logic/Map.h"
#include "Logic/Player.h"
#include "Utils.h"

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
int mapIndex = 0;		//El indice arbitrario del mapa que estamos usando, si es mayor que numMaps o menor que 0 falla

//Jugador
Player* playin = nullptr;

/*
* Lectura del mapa.
* abrir el archivo, si no se ha podido dejar de hacer cosas
* pillamos el numero de mapas a leer y creamos un array de mapas de ese tamanio
* cargamos un mapa y le pasamos el endianess. Si al cargar un mapa falla, se dejan de
* intentar cargas mas. 
* 
* Devuelve false si ha encontrado un error, lectura del fichero o lectura de un mapa
*/
bool lecturaMapa(const char* fileName) {
	bool bigEndian = Platform::IsBigEndian();

	FILE* file = Platform::OpenFile(fileName, "rb");
	if (file == nullptr) return false; // Ha fallado la carga

	// Numero de mapas
	if (fread(&numMaps, 2, 1, file) == 0) return false;
	if (bigEndian)
		numMaps = FLIPENDIAN_16((int)numMaps);

	maps = new Map[numMaps];
	bool loaded = true;
	int i = 0;
	while (i < numMaps && !feof(file) && loaded) {
		loaded = maps[i].load(file, bigEndian);
		i++;
	}
	if (!loaded) return false;

	
	return true;
}


void drawMap(const Map& map) {
	int finalTileSize = BASE_TILE_SIZE * scale;
	int centerX = Renderer::GetWidth() / 2, centerY = Renderer::GetHeight() / 2;
	for (int j = 0; j < map.getSize(); ++j) {
		if (map.getTile(j) <= 63 && map.getTile(j) > 0) {
			Renderer::DrawImage(*Renderer::GetImage(2* map.getTile(j) -2), 
				((j % BASE_TILE_SIZE) - playin->getPosX()) * finalTileSize + centerX,
				((j / BASE_TILE_SIZE) - playin->getPosY()) * finalTileSize + centerY,
				finalTileSize, 
				finalTileSize);
		}
	}
}

//crear al player en funcion del mapa
void createPlayer() {
	int sp = maps[mapIndex].getPlayerSpawnPoint();
	int dir = maps[mapIndex].getPlayerOrientation() % 19;
	playin = new Player((sp % BASE_TILE_SIZE) + 0.5f, (sp / BASE_TILE_SIZE) + 0.5f, DEG_RAD(((dir * 90) - 90)));
}

void drawPlayer() {
	// Línea
	int contactX = (Renderer::GetWidth() / 2) + (cos(playin->getAngle()) * PLAYER_BAR_SIZE); 
	int contactY = (Renderer::GetHeight() / 2) + (sin(playin->getAngle()) * PLAYER_BAR_SIZE);
	Renderer::DrawLine(Renderer::GetWidth() / 2, Renderer::GetHeight() / 2, contactX, contactY, { 255, 255, 255, 255 });

	// Jugador
	Renderer::DrawRect((Renderer::GetWidth() / 2) - (PLAYER_SIZE / 2),
		(Renderer::GetHeight() / 2) - (PLAYER_SIZE / 2),
		PLAYER_SIZE,
		PLAYER_SIZE,
		{ 255, 255, 0, 0 });
}

void draw() {
	drawMap(maps[mapIndex]);
	drawPlayer();
}

void update() {
	double deltaTime = Platform::getDeltaTime();

	float posX, posY;
	Input::GetAxis(posX, posY);
	float zoom = Input::GetZoom();

	if (zoom) {
		scale += zoom * deltaTime * ZOOM_PER_SECOND;
		clampf(scale,SCALE_MIN, SCALE_MAX);
	}
	
	if (!posX && !posY ) return; //si no hay movimiento no calcular cosas

	playin->calculateAngle(posX, posY);
	float realNextX = playin->getPosX() + (float)posX * TILES_PER_SECOND * deltaTime;
	float realNextY = playin->getPosY() + (float)posY * TILES_PER_SECOND * deltaTime;

	//esto para comprobar el siguiente tile
	// posX = roundAloAlto(posx) + roundAloBajo(posX)
	
	if (posX > 0)posX = 1;
	else if (posX < 0) posX = -1;
	if (posY > 0)posY = 1;
	else if (posY < 0) posY = -1;
	float nextX = playin->getPosX() + (float)posX * TILES_PER_SECOND * deltaTime;
	float nextY = playin->getPosY() + (float)posY * TILES_PER_SECOND * deltaTime;


	//te mueves en horizontal solo si ni en tu actual Y, ni en la siguiente hacia arriba del tile
	// ni en la de abajo del tile hay un tile
	int tmpX = std::floorf(nextX + (COLLISION_OFFSET * posX));
	if (maps[mapIndex].isTransitable(tmpX, playin->getPosY())
		&& maps[mapIndex].isTransitable(tmpX, playin->getPosY() + (COLLISION_OFFSET * posX))
		&& maps[mapIndex].isTransitable(tmpX, playin->getPosY() + (COLLISION_OFFSET * -posX))) {
		playin->setPosX(realNextX);
	}
	//te mueves en vertical similar a horizontal pero comprobaciones en la x
	int tmpY = std::floorf(nextY + (COLLISION_OFFSET * posY));
	if (maps[mapIndex].isTransitable(playin->getPosX(), tmpY) &&
		maps[mapIndex].isTransitable(playin->getPosX() + (COLLISION_OFFSET * posY), tmpY)
		&& maps[mapIndex].isTransitable(playin->getPosX() + (COLLISION_OFFSET * -posY), tmpY)) {
		playin->setPosY(realNextY);
	}
}

void free() {
	delete[] maps;
	delete playin;
}

int main(int argc, char* argv[])
{
	if (!Platform::Init() || !Renderer::Init(false, 1280, 720) || !Input::Init())
		return -1;

	Renderer::ReadImage("assets/walls.pak");

	if (!lecturaMapa("assets/maps.pak")) {
		free();
		return -1;
	}	

	createPlayer();


	while (Platform::Tick() /*&& cFrames++ < 100*/)
	{
		Input::Tick();
		update();
		Renderer::Clear({ 255, 0, 0, 0 });//limpiamos a color negro por defecto
		draw();
		Renderer::Present();
	}

	
	free();
	Renderer::Release();
	Platform::Release();

	return 0;
}