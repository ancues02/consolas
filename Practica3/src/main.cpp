#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include "Platform/Platform.h"
#include "Renderer/Renderer.h"
#include "Utils.h"
#include <stdio.h>

const int nameSize = 16;
const unsigned int baseTileSize = 20;
float scale = 2.0f;

uint16_t** vecMaps = nullptr;//64x64 informacion del mapa
char** vecNamesMaps = nullptr;//nombre de cada mapa
uint16_t** playerStart = nullptr;//3 x 1, posicion (x,y)del jugador y su orientacion
uint16_t numMaps = 0;

//TODO liberar memoria
void lecturaMapa(const char* fileName) {
	bool bigEndian = Platform::IsBigEndian();

	FILE* file = Platform::OpenFile(fileName, "rb");
	//std::ifstream* file;
	if (file == nullptr) return; // Ha fallado la carga

	uint16_t tile = 0, player = 0;// , numMaps = 0;
	
	//numero de mapas
	if (fread(&numMaps, 2, 1, file) == 0) return;
	if (bigEndian)
		numMaps = FLIPENDIAN_16((int)numMaps);
	vecMaps = new uint16_t * [numMaps];
	vecNamesMaps = new char* [numMaps];
	playerStart = new uint16_t*[numMaps];//posicion y orientacion del player en cada mapa

	for (int x = 0; x < numMaps && !feof(file); ++x) {
		//nombre del mapa
		
		//char name[nameSize];
		//for (int y = 0; y < nameSize; ++y) {
			//char tmp;
		vecNamesMaps[x] = new char[nameSize];
		if (fread(vecNamesMaps[x], 16, 1, file) == 0) return;
			//name[y] = tmp;
		//}
		//std::st
		//vecNamesMaps[x] = name;  
		std::cout << vecNamesMaps[x] << std::endl;

		//el mapa como tal, 2 planos de 64*64 el primero con informaicon del nivel
		// un tile con valor i se pinta en 2*i -1
		//el segundo con la informacion del player, solo quedarse con el valor entre 19 y 22 inclusives
		//19 player mira norte, 20 este, 21, sur, 22 oeste

		vecMaps[x] = new uint16_t[ 64 * 64];
		//primer plano, informacion del nivel (paredes)
		for (int y = 0; y < 64 * 64 ; ++y) {
			if (fread(&tile, sizeof(uint16_t), 1, file) == 0) break;
			if (bigEndian) tile = FLIPENDIAN_16(tile);
			vecMaps[x][y] = tile;		

		}

		playerStart[x] = new uint16_t[2];

		//segundo plano, informacion del jugador (posicion inicial y donde mira)
		for (int y = 0; y < 64 * 64 ; ++y) {
			if (fread(&player, sizeof(uint16_t), 1, file) == 0)
				break;
			if (bigEndian) player = FLIPENDIAN_16(player);
			if (player >= 19 && player <= 22) {
				playerStart[x][0] = y ;
				//playerStart[x][1] = y / 64;
				playerStart[x][1] = player;
			}
		}

	}
}

float toRad(float deg) {
	return (deg * 3.14) / 180;
}
void drawMap(char* nameMap) {
	int i = 0;
	//buscar que mapa es
	for (; i < numMaps; ++i) {
		if (std::strcmp(vecNamesMaps[i], nameMap) == 0)
			break;
		std::cout << vecNamesMaps[i] << std::endl;
	}

	if (i > numMaps)
		//mapa no encontrado
		return;
	int posPlayer = playerStart[i][0];
	int finalTileSize = baseTileSize * scale;
	for (int j = 0; j < 64 * 64; ++j) {
		if (vecMaps[i][j] <= 63 && vecMaps[i][j] > 0) {
			//std::cout << "tile " << tile << std::endl;
			Renderer::DrawImage(*Renderer::GetImage(2*vecMaps[i][j] -2), ((j % 64) - (posPlayer % 64) ) * finalTileSize + Renderer::GetWidth() / 2,
				((j / 64) - (posPlayer / 64) ) * finalTileSize + Renderer::GetHeight() / 2, finalTileSize, finalTileSize);
		}
	}
}

void draw() {
	

	//Renderer::DrawImage(*Renderer::GetImage(0), 0, 0, 100, 100);
	drawMap("Wolf1 Map1");
}

// Cambia los valores de la logica, para mover el grid, el angulo de la linea y el color del cuadrado
void updateLogic() {
	
}

int main(int argc, char* argv[])
{
	//std::ofstream results; std::string fileDesc = "1280x720NoVSYNC";
	//results.open("results" + fileDesc + ".txt", std::ios::trunc);	

	Platform::Init();
	Renderer::Init(false, 1920, 1080);
	
	//int cFrames = 0;
	//auto start = std::chrono::high_resolution_clock::now();

	Renderer::ReadImage("assets/walls.pak");
	lecturaMapa("assets/maps.pak");
	while (Platform::Tick() /* && cFrames++ < totalFrames*/)
	{
		Renderer::Clear({ 255, 0, 0, 0 });//limpiamos a color negro por defecto
		//Renderer::Clear(colors[ind]);//pruebas para ver el tearing
		//parseInput()
		updateLogic(/*dameinput*/);
		draw();
		Renderer::Present();

	}
	//Para las medidas
	//auto end = std::chrono::high_resolution_clock::now();
	//auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	//std::cout << duration.count() << " ms for " << offsets[i] << " grid offset." << std::endl;
	

	Renderer::Release();
	Platform::Release();


	//results.close();
	return 0;
}