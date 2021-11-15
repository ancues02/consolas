#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include "Platform/Platform.h"
#include "Renderer/Renderer.h"
#include "Utils.h"
#include <stdio.h>

const int nameSize = 16;

int** vecMaps = nullptr;

float toRad(float deg) {
	return (deg * 3.14) / 180;
}

void lecturaMapa(const char* fileName) {
	bool bigEndian = Platform::IsBigEndian();

	FILE* file = Platform::OpenFile(fileName, "r");

	if (file == nullptr) return; // Ha fallado la carga
	int tile, numMaps;
	//numero de mapas
	if (fread(&numMaps, 2, 1, file) == 0) return;
	if (bigEndian)
		numMaps = FLIPENDIAN_32((int)numMaps);
	vecMaps = new int* [numMaps];

	char name[nameSize];
	if (fread(&name, 16, 1, file) == 0) return;

	for (int x = 0; x < numMaps && !feof(file); ++x) {
		vecMaps[x] = new int[64 * 64];
		for (int y = 0; y < 64 * 64; ++y) {
			if (fread(&tile, 2, 1, file) == 0) break;
			if (bigEndian) tile = FLIPENDIAN_16(tile);
			vecMaps[x][y] = tile;
		}
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