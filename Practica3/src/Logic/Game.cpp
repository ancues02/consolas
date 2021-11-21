#include "Game.h"

#include <cmath>

#include "../Platform/Platform.h"
#include "../Renderer/Renderer.h"
#include "../Input/Input.h"

#include "Map.h"
#include "Player.h"

Game::Game()
{
}

Game::~Game()
{
	free();
}



void Game::draw() {
	drawMap(maps[mapIndex]);
	drawPlayer();
}


bool Game::loadMaps(const char* fileName)
{
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

void Game::setMap(int indx)
{
	if (indx >= 0 && indx < numMaps) {
		mapIndex = indx;
		int sp = maps[mapIndex].getPlayerSpawnPoint();
		int dir = maps[mapIndex].getPlayerOrientation() % 19;
		playin = new Player((sp % TILE_SIZE) + 0.5f, (sp / TILE_SIZE) + 0.5f, DEG_RAD(((dir * 90) - 90)));
	}
}

void Game::setScaleLimits(float MinScale, float MaxScale)
{
	if(minScale <= MaxScale)
		minScale = MinScale; maxScale = MaxScale;
}

void Game::update() {
	double deltaTime = Platform::getDeltaTime();

	float posX = Input::GetHorizontalAxis();
	float posY = Input::GetVerticalAxis();
	float zoom = Input::GetZoom();

	if (zoom) {
		scale += zoom * deltaTime * ZOOM_SPEED;
		clampf(scale, minScale, maxScale);
	}

	if (!posX && !posY) return; //si no hay movimiento no calcular cosas

	playin->calculateAngle(posX, posY);
	float realNextX = playin->getPosX() + posX * PLAYER_SPEED * deltaTime;
	float realNextY = playin->getPosY() + posY * PLAYER_SPEED * deltaTime;

	//esto para comprobar el siguiente tile
	// posX = roundAloAlto(posx) + roundAloBajo(posX)

	if (posX > 0)posX = 1;
	else if (posX < 0) posX = -1;
	if (posY > 0)posY = 1;
	else if (posY < 0) posY = -1;
	float nextX = playin->getPosX() + posX * PLAYER_SPEED * deltaTime;
	float nextY = playin->getPosY() + posY * PLAYER_SPEED * deltaTime;


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

void Game::drawMap(const Map& map) {
	int finalTileSize = TILE_SIZE * scale;
	int centerX = Renderer::GetWidth() / 2, centerY = Renderer::GetHeight() / 2;
	for (int j = 0; j < map.getSize(); ++j) {
		if (map.getTile(j) <= 63 && map.getTile(j) > 0) {
			Renderer::DrawImage(*Renderer::GetImage(2 * map.getTile(j) - 2),
				((j % TILE_SIZE) - playin->getPosX()) * finalTileSize + centerX,
				((j / TILE_SIZE) - playin->getPosY()) * finalTileSize + centerY,
				finalTileSize,
				finalTileSize);
		}
	}
}

void Game::drawPlayer()
{
	// L�nea
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

void Game::free() {
	delete[] maps;
	delete playin;
}