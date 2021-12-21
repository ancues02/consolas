#include "Input/Input.h"
#include "Platform/Platform.h"
#include "Renderer/Renderer.h"
#include "Logic/Game.h"
#include "Input/Input.h"
#include "Renderer/RenderThread.h"

#include <condition_variable>
#include <mutex>
#include <iostream>

#ifdef PLATFORM_PS4
#include <razorcpu.h>
#include <razorcpu_debug.h>
#endif

int main(int argc, char* argv[])
{
#ifdef PLATFORM_PS4
	scePthreadSetaffinity(scePthreadSelf(), 1 << 5);
#endif

	if (!Platform::Init())
		return -1;
	if (!Renderer::Init(false, 1920, 1080)) {
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
	int frameCount = 0;
	float duration = 0;
	while (Platform::Tick())
	{
		Input::Tick();
		game.update();
		game.draw();

		// Esperamos si hay demasiados frames encolados (logica mas rapida que render)
		if (RenderThread::GetFrames() > RenderThread::GetMaxQueuedFrames()) {
			RenderThread::WaitLock();
		}
		RenderThread::AddCommand(rCPresent);
		RenderThread::IncreaseFrames();
		RenderThread::SignalLock();
		frameCount++;
		duration += Platform::getDeltaTime();
		if (duration > 5) {
			std::cout << frameCount / duration << std::endl;
			frameCount = duration = 0;
		}
#ifdef PLATFORM_PS4
		sceRazorCpuSync();
#endif
	}
	RenderThread::Stop();
	Input::Release();
	Renderer::Release();
	Platform::Release();

	return 0;
}