#include "Game.h"

#include <cmath>

#include "../Platform/Platform.h"
#include "../Renderer/Renderer.h"
#include "../Input/Input.h"

#include "Map.h"
#include "Player.h"
#include "Raycaster.h"

Game::Game()
{
}

Game::~Game()
{
	free();
}

void Game::draw() {
	drawMap(maps[mapIndex]);
	drawRays();
	drawPlayer();
}

bool Game::Init(const char* map, int index) {
	if (!loadMaps(map)) return false;
	playin = new Player(0, 0, 0);
	if (!setMap(index)) return false;
	ray = new Raycaster();
	return true;
}

bool Game::loadMaps(const char* fileName)
{
	bool bigEndian = Platform::IsBigEndian();

	FILE* file = Platform::OpenFile(fileName, "rb");
	if (file == nullptr) return false; // Ha fallado la carga

	// Numero de mapas
	if (fread(&numMaps, 2, 1, file) == 0) {
		Platform::CloseFile(fileName);
		return false;
	}
	if (bigEndian)
		numMaps = FLIPENDIAN_16((int)numMaps);

	maps = new Map[numMaps];
	bool loaded = true;
	int i = 0;
	while (i < numMaps && !feof(file) && loaded) {
		loaded = maps[i].load(file, bigEndian);
		i++;
	}
	Platform::CloseFile(fileName);

	if (!loaded) return false;

	return true;
}

bool Game::setMap(int indx)
{
	if (indx >= 0 && indx < numMaps) {
		mapIndex = indx;
		int sp = maps[mapIndex].getPlayerSpawnPoint();
		int dir = maps[mapIndex].getPlayerOrientation() % 19;
		playin->setPosX((sp % TILE_SIZE) + 0.5f);
		playin->setPosY((sp / TILE_SIZE) + 0.5f);
		playin->setAngle(DEG_RAD(((dir * 90) - 90)));
		return true;
	}
	else return false;
}

void Game::setScaleLimits(float MinScale, float MaxScale)
{
	if(minScale <= MaxScale)
		minScale = MinScale; maxScale = MaxScale;
}

void Game::update() {
	
	double deltaTime = Platform::getDeltaTime();

	float posX, posY;
	Input::GetAxis(posX, posY);

	playin->rotate(posX * deltaTime);

	if (posY) {

		float deltaSpeed = playin->getSpeed() * deltaTime;
		float moveX = - posY * cos(playin->getAngle()) * deltaSpeed;
		float moveY = - posY * sin(playin->getAngle()) * deltaSpeed;

		int dirX = moveX > 0 ? 1 : ((moveX < 0) ? -1 : 0);
		int dirY = moveY > 0 ? 1 : ((moveY < 0) ? -1 : 0);

		if (maps[mapIndex].isTransitable(playin->getPosX(), playin->getPosY() + moveY + dirY * COLLISION_OFFSET) &&
			maps[mapIndex].isTransitable(playin->getPosX() + COLLISION_OFFSET, playin->getPosY() + moveY + dirY * COLLISION_OFFSET) && 
			maps[mapIndex].isTransitable(playin->getPosX() - COLLISION_OFFSET, playin->getPosY() + moveY + dirY * COLLISION_OFFSET))
			playin->setPosY(playin->getPosY() + moveY);

		if (maps[mapIndex].isTransitable(playin->getPosX() + moveX + dirX * COLLISION_OFFSET, playin->getPosY()) &&
			maps[mapIndex].isTransitable(playin->getPosX() + moveX + dirX * COLLISION_OFFSET, playin->getPosY() + COLLISION_OFFSET) && 
			maps[mapIndex].isTransitable(playin->getPosX() + moveX + dirX * COLLISION_OFFSET, playin->getPosY() - COLLISION_OFFSET))
			playin->setPosX(playin->getPosX() + moveX);
	}
	

	ray->CastRays(playin->getPosX(), playin->getPosY(), playin->getAngle(), DEG_RAD(90), maps[mapIndex]);
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

void Game::drawRays() {
	RaycastData* rays = ray->getRays();
	int finalTileSize = TILE_SIZE * scale;
	int centerX = Renderer::GetWidth() / 2,
		centerY = Renderer::GetHeight() / 2;
	for (int i = 0; i < NUMBER_OF_RAYS; ++i) {
		Renderer::DrawLine(Renderer::GetWidth() / 2, Renderer::GetHeight() / 2, 
			(rays[i].posX - playin->getPosX()) * finalTileSize + centerX, 
			(rays[i].posY - playin->getPosY()) * finalTileSize + centerY,
			rays[i].vertical ? Color{ 255, 0, 255, 0 } : Color{ 255, 255, 0, 0 });
	}
}

void Game::drawPlayer()
{
	// L�nea
	int contactX = (Renderer::GetWidth() / 2) + (cos(playin->getAngle()) * playin->getBarSize());
	int contactY = (Renderer::GetHeight() / 2) + (sin(playin->getAngle()) * playin->getBarSize());
	Renderer::DrawLine(Renderer::GetWidth() / 2, Renderer::GetHeight() / 2, contactX, contactY, { 255, 255, 255, 255 });

	// Jugador
	Renderer::DrawRect((Renderer::GetWidth() / 2) - (playin->getSize() / 2),
		(Renderer::GetHeight() / 2) - (playin->getSize() / 2),
		playin->getSize(),
		playin->getSize(),
		{ 255, 255, 0, 0 });
}

void Game::free() {
	delete[] maps;
	delete playin;
	delete ray;
}
