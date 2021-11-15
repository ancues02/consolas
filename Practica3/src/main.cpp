#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include "Platform/Platform.h"
#include "Renderer/Renderer.h"
#include "Utils.h"
#include <stdio.h>

const int nameSize = 16;

int** vecMaps = nullptr;//64x64 informacion del mapa
char* vecNamesMaps = nullptr;//nombre de cada mapa
int* playerStart = nullptr;//2 x 1, posicion del jugador y su orientacion

float toRad(float deg) {
	return (deg * 3.14) / 180;
}

void lecturaMapa(const char* fileName) {
	bool bigEndian = Platform::IsBigEndian();

	FILE* file = Platform::OpenFile(fileName, "r");

	if (file == nullptr) return; // Ha fallado la carga

	uint16_t tile, player, numMaps;
	
	//numero de mapas
	if (fread(&numMaps, 2, 1, file) == 0) return;
	if (bigEndian)
		numMaps = FLIPENDIAN_16((int)numMaps);
	vecMaps = new int* [numMaps];
	vecNamesMaps = new char[numMaps];
	playerStart = new int[2];//posicion y orientacion del player

	for (int x = 0; x < numMaps && !feof(file); ++x) {
		//nombre del mapa
		char name[nameSize];
		if (fread(&name, 16, 1, file) == 0) return;
		vecNamesMaps[x] = *name;

		//el mapa como tal, 2 planos de 64*64 el primero con informaicon del nivel
		// un tile con valor i se pinta en 2*i -1
		//el segundo con la informacion del player, solo quedarse con el valor entre 19 y 22 inclusives
		//19 player mira norte, 20 este, 21, sur, 22 oeste

		vecMaps[x] = new int[ 64 * 64];
		//primer plano, informacion del nivel (paredes)
		for (int y = 0; y < 64 * 64 ; ++y) {
			if (fread(&tile, 2, 1, file) == 0) break;
			if (bigEndian) tile = FLIPENDIAN_16(tile);
			vecMaps[x][y] = tile;
			if (tile <= 63 && tile >0) {
				//std::cout << "tile " << tile << std::endl;

			}
			else
				std::cout << "tile " << tile << std::endl;

		}
		//segundo plano, informacion del jugador (posicion inicial y donde mira)
		for (int y = 0; y < 64 * 64 ; ++y) {
			if (fread(&player, 2, 1, file) == 0) break;
			if (bigEndian) player = FLIPENDIAN_16(player);
			if (player >= 19 && player <= 22) {
				playerStart[0] = y;
				playerStart[1] = player;
				break;
			}
			else
				std::cout << "player " << player << std::endl;

		}
		std::cout << "player " << player << std::endl;

	}
}

void draw() {
	// GRID
	/*int horizontal = gridOffset;
	while (horizontal < Renderer::GetWidth()) {
		Renderer::DrawLine(horizontal , 0, horizontal , Renderer::GetHeight(), { 255, 255, 255, 255 });
		horizontal += offsets[gridInd];
	}

	int vertical = gridOffset;
	while (vertical < Renderer::GetHeight()) {
		Renderer::DrawLine(0, vertical , Renderer::GetWidth(), vertical , { 255, 255, 255, 255 });
		vertical += offsets[gridInd];
	}

	// CIRCLE
	int rad = Renderer::GetHeight() * 2 / 6;
	Renderer::DrawCircle(Renderer::GetWidth() / 2, Renderer::GetHeight() / 2, rad, { 255, 255, 255, 255 });
	
	// SQUARE LINE
	int contactX = Renderer::GetWidth() / 2 + cos(toRad(boxAngle)) * rad; int contactY = Renderer::GetHeight() / 2 + sin(toRad(boxAngle)) * rad;
	Renderer::DrawLine(Renderer::GetWidth() / 2, Renderer::GetHeight() / 2, contactX, contactY, { 255, 255, 255, 255 });

	// SQUARE
	Renderer::DrawRect(contactX - squareSize / 2, contactY - squareSize / 2, squareSize, squareSize, colors[ind]);*/

	Renderer::DrawImage(*Renderer::GetImage(0), 0, 0, 200, 100);
}

// Cambia los valores de la logica, para mover el grid, el angulo de la linea y el color del cuadrado
void updateLogic() {
	
}

/*
	handleinput(){
	Input::parseInput
	poll(){
	if(MoveX)
		movement *= -1;
	
	}
	if(Evt.Exit)
		exit = true;
*/
int main(int argc, char* argv[])
{
	//std::ofstream results; std::string fileDesc = "1280x720NoVSYNC";
	//results.open("results" + fileDesc + ".txt", std::ios::trunc);	

	Platform::Init();
	Renderer::Init(false, 1280, 720);
	
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