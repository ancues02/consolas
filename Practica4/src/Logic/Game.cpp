#include "Game.h"

#include <cmath>

#include "../Platform/Platform.h"
#include "../Renderer/Renderer.h"
#include "../Input/Input.h"

#include "Map.h"
#include "Player.h"
#include "../Renderer/RenderThread.h"
#include "Raycaster.h"

#include <iostream>

Game::Game()
{
}

Game::~Game()
{
	free();
}

void Game::draw() {
	drawBack();
	//drawMap(maps[mapIndex]);
	//drawRays();
	//drawPlayer();
}

bool Game::Init(const char* map, int index) {
	if (!loadMaps(map)) return false;
	playin = new Player(0, 0, 0);
	if (!setMap(index)) return false;
	ray = new Raycaster(Renderer::GetHeight(), Renderer::GetWidth());
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

void Game::drawBack() {

	RenderCommand rCClearTop;
	rCClearTop.tipo = RenderCommandType::CLEAR_RECT;
	rCClearTop.clearRectInfo.color = Color{ 255,150,150,150 };
	rCClearTop.clearRectInfo.x1 = 0;
	rCClearTop.clearRectInfo.y1 = 0;
	rCClearTop.clearRectInfo.x2 = Renderer::GetWidth();
	rCClearTop.clearRectInfo.y2 = Renderer::GetHeight() / 2;

	RenderThread::addCommand(rCClearTop);

	RenderCommand rCClearBot;
	rCClearBot.tipo = RenderCommandType::CLEAR_RECT;
	rCClearBot.clearRectInfo.color = Color{ 255,128,128,128 };
	rCClearBot.clearRectInfo.x1 = 0;
	rCClearBot.clearRectInfo.y1 = Renderer::GetHeight() / 2;
	rCClearBot.clearRectInfo.x2 = Renderer::GetWidth();
	rCClearBot.clearRectInfo.y2 = Renderer::GetHeight() / 2;

	RenderThread::addCommand(rCClearBot);

	//Renderer::DrawRect(0, 0, Renderer::GetWidth(), Renderer::GetHeight() / 2, Color{255, 0, 0, 0});
	//Renderer::DrawRect(0, Renderer::GetHeight() / 2, Renderer::GetWidth(), Renderer::GetHeight() / 2, Color{255, 128, 128, 128});
}

void Game::drawMap(const Map& map) {
	int h = Renderer::GetHeight();
	int w = Renderer::GetWidth();

	RaycastData* data = ray->getRays();
	for (int i = 0; i < Renderer::GetWidth(); ++i) {
		//Calculate height of line to draw on screen
		int lineHeight = (int)(h / data[i].distance);

		int pitch = 0;

		//calculate lowest and highest pixel to fill in current stripe
		int drawStart = -lineHeight / 2 + h / 2 + pitch;
		int drawEnd = lineHeight / 2 + h / 2 + pitch;

		int j = data[i].mapX + (data[i].mapY * map.getWidth());
		if (data[i].mapX < 0 || data[i].mapY < 0) {
			int x = 0;
		}

		//calculate value of wallX
		float wallX; //where exactly the wall was hit
		if (data[i].side == 0)
			wallX = data[i].posY + data[i].distance * data[i].rayDirY;
		else
			wallX = data[i].posX + data[i].distance * data[i].rayDirX;		
		wallX -= floor(wallX);

		//x coordinate on the texture
		int texX = int(wallX * double(TILE_SIZE));
		if (data[i].side == 0 && data[i].rayDirX > 0)
			texX = TILE_SIZE - texX - 1;
		if (data[i].side == 1 && data[i].rayDirY < 0)
			texX = TILE_SIZE - texX - 1;

		//Renderer::DrawLine(i, drawStart, i, drawEnd, data[i].vertical ? Color({ 255, 0, 255, 0 }) : Color({ 255, 255, 0, 0 }));
		RenderCommand rC;
		rC.tipo = RenderCommandType::DRAW_TEXTURE;
		rC.drawTextureLineInfo.image = Renderer::GetImage(2 * map.getTile(j) - (1 + data[i].side));
		rC.drawTextureLineInfo.texX = texX;
		rC.drawTextureLineInfo.x1 = i;
		rC.drawTextureLineInfo.y1 = drawStart;
		rC.drawTextureLineInfo.x2 =1;
		rC.drawTextureLineInfo.y2 = drawEnd - drawStart;


		//Renderer::DrawImageColumn(*Renderer::GetImage(2 * map.getTile(j) - (1 + data[i].side)),
		//	texX, i, drawStart, 1, drawEnd - drawStart);
	}
}

void Game::drawRays() {
	RaycastData* rays = ray->getRays();
	int finalTileSize = TILE_SIZE * scale;
	int centerX = Renderer::GetWidth() / 2,
		centerY = Renderer::GetHeight() / 2;
	for (int i = 0; i < Renderer::GetWidth(); ++i) {
		Renderer::DrawLine(Renderer::GetWidth() / 2, Renderer::GetHeight() / 2, 
			(rays[i].posX - playin->getPosX()) * finalTileSize + centerX, 
			(rays[i].posY - playin->getPosY()) * finalTileSize + centerY,
			rays[i].side ? Color{ 255, 0, 255, 0 } : Color{ 255, 255, 0, 0 });
	}
}

void Game::drawPlayer()
{
	// Linea
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
