#include "Map.h"
#include "../Utils.h"

Map::Map(int width, int heigth, int nameSize) :
	_width(width),
	_heigth(heigth),
	_size(width * heigth),
	_nameSize(nameSize),
	_mapInfo(nullptr),
	_name(nullptr),
	_playerStart(nullptr)
{
}

Map::Map() : Map(64, 64, 16)
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
	int size = _width * _heigth;

	_name = new char[_nameSize];
	_mapInfo = new uint16_t[size];
	_playerStart = new uint16_t[2];

	if (fread(_name, _nameSize, 1, file) == 0) return false;

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
	return _width * _heigth;
}

int Map::getWidth() const
{
	return _width;
}

int Map::getHeight() const
{
	return _heigth;
}

const char* Map::getName() const
{
	return _name;
}

bool Map::isTransitable(int x, int y) const
{
	return getTile(x + y * _width) < 1 || getTile(x + y * _width) > 63;
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
	if (i >= 0 && i < _size)
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
