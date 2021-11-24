#include "Map.h"
#include "../Utils.h"

Map::Map()
{
}

Map::~Map()
{
	free();
}

bool Map::load(FILE* file, bool bigEndian)
{
	if (file == nullptr) return false;

	uint16_t tile = 0;
	int size = MAPWIDTH * MAPHEIGHT;

	_name = new char[MAPNAMESIZE];
	_mapInfo = new uint16_t[size];
	_playerStart = new uint16_t[2];

	if (fread(_name, MAPNAMESIZE, 1, file) == 0) return false;

	//primer plano, informacion del nivel (paredes)
	for (int y = 0; y < size; ++y) {
		if (fread(&tile, sizeof(uint16_t), 1, file) == 0) 
			return false;
		if (bigEndian) tile = FLIPENDIAN_16(tile);
		_mapInfo[y] = tile;
	}

	//segundo plano, informacion del jugador (posicion inicial y donde mira)
	for (int y = 0; y < size; ++y) {
		if (fread(&tile, sizeof(uint16_t), 1, file) == 0)
			return false;
		if (bigEndian) tile = FLIPENDIAN_16(tile);
		if (tile >= 19 && tile <= 22) {
			_playerStart[0] = y;
			_playerStart[1] = tile;
		}
	}

	return true;
}

int Map::getSize() const
{
	return MAPWIDTH * MAPHEIGHT;
}

int Map::getWidth() const
{
	return MAPWIDTH;
}

int Map::getHeight() const
{
	return MAPHEIGHT;
}

const char* Map::getName() const
{
	return _name;
}

bool Map::isTransitable(int x, int y) const
{
	return getTile(x + y * MAPWIDTH) < 1 || getTile(x + y * MAPWIDTH) > 63;
}

uint16_t Map::getPlayerSpawnPoint() const
{
	return _playerStart[0];
}

uint16_t Map::getPlayerOrientation() const
{
	return _playerStart[1];
}

uint16_t Map::getTile(int i) const
{
	if (i >= 0 && i < MAPSIZE)
		return _mapInfo[i];
	return 0;
}

void Map::free()
{
	delete [] _name; 
	_name = nullptr;
	delete [] _mapInfo; 
	_mapInfo = nullptr;
	delete [] _playerStart; 
	_playerStart = nullptr;
}
