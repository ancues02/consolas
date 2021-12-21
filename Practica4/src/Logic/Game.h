#pragma once
#include <cstdint>
#include "../Renderer/RenderCommand.h"

class Map;
class Player;
class Raycaster;

class Game {
public:
	Game();
	~Game();

	/*
	 * Lectura del mapa.
	 * abrir el archivo, si no se ha podido dejar de hacer cosas
	 * pillamos el numero de mapas a leer y creamos un array de mapas de ese tamanio
	 * cargamos un mapa y le pasamos el endianess. Si al cargar un mapa falla, se dejan de
	 * intentar cargas mas.
	 *
	 * Devuelve false si ha encontrado un error, lectura del fichero o lectura de un mapa
	 */
	bool Init(const char* map, int index);
	bool loadMaps(const char* fileName);
	bool setMap(int indx);
	void setScaleLimits(float MinScale, float MaxScale);

	void update();
	void draw();

private:
	// Constantes
	const unsigned int TILE_SIZE = 64;
	const float ZOOM_SPEED = 1;
	const float FIELD_OF_VIEW = 90;	// En degrees!!

	const float COLLISION_OFFSET = 0.2f;

	// Variables del mapa
	float maxScale = 3.0f;
	float minScale = 0.5f;

	// Mapas
	uint16_t numMaps = 0;	//numero de mapas que hay
	Map* maps = nullptr;	//todos los mapas
	int mapIndex = 0;		//El indice arbitrario del mapa que estamos usando, si es mayor que numMaps o menor que 0 falla

	// Jugador
	Player* playin = nullptr;

	// Raycaster
	Raycaster* ray = nullptr;

	// Escalado
	float scale = 1.0f;

	// Comandos para hacer el clear
	RenderCommand rCClearTop;
	RenderCommand rCClearBot;

	// Dibujado de la P4
	void drawBack();
	void drawMap3D(const Map&);
	void drawRays();

	// Dibujado de la P3
	void drawMap(const Map&);
	void drawPlayer();

	// Libera el mapa, el jugadro y el raycaster
	void free();
};