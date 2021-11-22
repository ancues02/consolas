#include "Input/Input.h"
#include "Platform/Platform.h"
#include "Renderer/Renderer.h"
#include "Logic/Game.h"
#include "Input/Input.h"
#include "Logic/Map.h"
#include "Logic/Player.h"
#include "Utils.h"

int main(int argc, char* argv[])
{
	if (!Platform::Init() || !Renderer::Init(false, 1280, 720) || !Input::Init())
		return -1;

	Game game;
	if (!Renderer::ReadImage("assets/walls.pak") || !game.loadMaps("assets/maps.pak"))
		return -1;

	game.setMap(0);
	while (Platform::Tick() /*&& cFrames++ < 100*/)
	{
		Input::Tick();
		game.update();
		Renderer::Clear({ 255, 0, 0, 0 });//limpiamos a color negro por defecto
		game.draw();
		Renderer::Present();
	}
	
	Input::Release();
	Renderer::Release();
	Platform::Release();

	return 0;
}