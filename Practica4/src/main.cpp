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
	RenderCommand rCClearTop;
	rCClearTop.tipo = RenderCommandType::CLEAR_RECT;
	rCClearTop.clearRectInfo.color = Color{ 255,0,0,0 };
	rCClearTop.clearRectInfo.x1 = 0;
	rCClearTop.clearRectInfo.y1 = 0;
	rCClearTop.clearRectInfo.x2 = Renderer::GetWidth();
	rCClearTop.clearRectInfo.y2 = Renderer::GetHeight() / 2;

	RenderCommand rCClearBot;
	rCClearBot.tipo = RenderCommandType::CLEAR_RECT;
	rCClearBot.clearRectInfo.color = Color{ 255,128,128,128 };
	rCClearBot.clearRectInfo.x1 = 0;
	rCClearBot.clearRectInfo.y1 = Renderer::GetHeight() / 2;
	rCClearBot.clearRectInfo.x2 = Renderer::GetWidth();
	rCClearBot.clearRectInfo.y2 = Renderer::GetHeight() / 2;

	RenderCommand rCPresent;
	rCPresent.tipo = RenderCommandType::PRESENT_FRAME;

	while (Platform::Tick())
	{
		Input::Tick();
		game.update();
		RenderThread::addCommand(&rCClearTop);
		RenderThread::addCommand(&rCClearBot);
		game.draw();
		RenderThread::addCommand(&rCPresent);

	}
	RenderThread::Stop();
	Input::Release();
	Renderer::Release();
	Platform::Release();

	return 0;
}