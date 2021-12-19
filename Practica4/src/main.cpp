#include "Input/Input.h"
#include "Platform/Platform.h"
#include "Renderer/Renderer.h"
#include "Logic/Game.h"
#include "Input/Input.h"
#include "Renderer/RenderThread.h"


int main(int argc, char* argv[])
{
	if (!Platform::Init())
		return -1;
	if (!Renderer::Init(false, 1280, 720)) {
		Platform::Release();
		return -1;
	}
	if (!Input::Init()) {
		Platform::Release();
		Renderer::Release();
		return -1;
	}

	Game game;
	if (!Renderer::ReadImage("assets/walls.pak") || !game.Init("assets/maps.pak", 0)) {
		Input::Release();
		Renderer::Release();
		Platform::Release();
		return -1;
	}


	//while (Platform::Tick())
	//{
	//	Input::Tick();
	//	game.update();
	//	Renderer::Clear({ 255, 0, 0, 0 });//limpiamos a color negro por defecto
	//	game.draw();
	//	Renderer::Present();
	//}
	
	
	RenderThread::Start();

	RenderCommand rCPresent;
	rCPresent.tipo = RenderCommandType::PRESENT_FRAME;

	while (Platform::Tick())
	{
		while (RenderThread::getFrames() > 2) {
			//std::this_thread::sleep_for(std::chrono::microseconds(1));
			//continue;
		}
		Input::Tick();
		game.update();
		game.draw();
		//RenderThread::addCommand(&rCClearTop);
		//RenderThread::addCommand(&rCClearBot);
		//game.draw();
		RenderThread::addCommand(rCPresent);

	}
	RenderThread::Stop();
	Input::Release();
	Renderer::Release();
	Platform::Release();

	return 0;
}