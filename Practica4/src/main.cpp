#include "Input/Input.h"
#include "Platform/Platform.h"
#include "Renderer/Renderer.h"
#include "Logic/Game.h"
#include "Input/Input.h"
#include "Renderer/RenderThread.h"

#include <condition_variable>
#include <mutex>
#include <iostream>

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
	
	RenderThread::Start();

	RenderCommand rCPresent;
	rCPresent.tipo = RenderCommandType::PRESENT_FRAME;

	while (Platform::Tick())
	{
		if (RenderThread::getFrames() > RenderThread::maxEnculados) {
			std::unique_lock<std::mutex> lock(RenderThread::_mutex);
			RenderThread::_cv.wait(lock);
		}
		Input::Tick();
		game.update();
		game.draw();
		RenderThread::addCommand(rCPresent);
		std::cout << 1.0f / (Platform::getDeltaTime() + 0.0000000001) << std::endl;
	}
	RenderThread::Stop();
	Input::Release();
	Renderer::Release();
	Platform::Release();

	return 0;
}