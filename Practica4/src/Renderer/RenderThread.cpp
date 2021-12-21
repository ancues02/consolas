#include "../Queue/concurrent_queue.h"

#include "RenderThread.h"
#include <cstdint>
#include "../Renderer/Renderer.h"
#include <chrono>

#ifdef PLATFORM_PS4
#include <razorcpu.h>
#include <razorcpu_debug.h>
#endif

std::thread RenderThread::_renderThread;
std::atomic<bool> RenderThread::_exit;
std::atomic<unsigned short> RenderThread::_frames;
ConcurrentQueue<RenderCommand> RenderThread::_q;

std::condition_variable RenderThread::_cv;
std::mutex RenderThread::_mutex;
std::unique_lock<std::mutex> RenderThread::_lock(_mutex);

const unsigned short RenderThread::maxQueue = 2;

void RenderThread::Start()
{
	//crea el hilo y llama a run desde el hilo
	_exit = false;
	_frames = 0;
	_renderThread = std::thread(run);
}

void RenderThread::Stop()
{
	_exit = true;

	// Por si la therad estaba bloqueada
	RenderCommand com;
	com.tipo = RenderCommandType::PRESENT_FRAME;
	_q.Enqueue(com);
	IncreaseFrames();
	SignalLock();

	// Esperar a que termine el hilo
	_renderThread.join();
}

void RenderThread::IncreaseFrames() {
	_frames++;
}

unsigned short RenderThread::GetFrames() {
	return _frames;
}

unsigned short RenderThread::GetMaxQueuedFrames()
{
	return maxQueue;
}

void RenderThread::AddCommand(const RenderCommand& command)
{
	_q.Enqueue(command);
}

void RenderThread::run()
{
#ifdef PLATFORM_PS4
	scePthreadSetaffinity(scePthreadSelf(), 1 << 3);
#endif
	while (!_exit) {
		// Esperamos si no hay comandos que procesar (logica mas lenta que render)
		if (_q.size() <= 0) {
			WaitLock();
		}
		RenderCommand c;		
		_q.Dequeue(c);

		// Procesar los comandos para renderizar un frame
		while (c.tipo != RenderCommandType::PRESENT_FRAME) {
			switch (c.tipo)
			{
			case(RenderCommandType::DRAW_RECT):
#ifdef PLATFORM_PS4
				sceRazorCpuPushMarker("DRAW_RECT", SCE_RAZOR_CPU_COLOR_GREEN, 1);
#endif
				Renderer::DrawRect(c.drawRectInfo.x1, c.drawRectInfo.y1, c.drawRectInfo.x2, c.drawRectInfo.y2, c.drawRectInfo.color);
#ifdef PLATFORM_PS4
				sceRazorCpuPopMarker();
#endif
				break;
			case(RenderCommandType::DRAW_TEXTURE):

				Renderer::DrawImageColumn(*c.drawTextureLineInfo.image,
					c.drawTextureLineInfo.texX, c.drawTextureLineInfo.x1, c.drawTextureLineInfo.y1, c.drawTextureLineInfo.x2, c.drawTextureLineInfo.y2);

				break;
			case(RenderCommandType::RAY_LINES):
#ifdef PLATFORM_PS4
				sceRazorCpuPushMarker("RAYRENDERING", SCE_RAZOR_CPU_COLOR_BLUE, 2);
#endif
				for (int i = 0; i < c.rayLinesInfo.wRays; i++) {
					Renderer::DrawImageColumn(*c.rayLinesInfo.rl[i].im,
						c.rayLinesInfo.rl[i].texX, c.rayLinesInfo.rl[i].x1, c.rayLinesInfo.rl[i].y1, c.rayLinesInfo.rl[i].x2, c.rayLinesInfo.rl[i].y2);
				}

				// Borramos la informacion del raycaster del comando
				delete[] c.rayLinesInfo.rl;
#ifdef PLATFORM_PS4
				sceRazorCpuPopMarker();
#endif
				break;
			default:
				break;
			}
			_q.Dequeue(c);			
		}

		// Hacer el flip cuando el frame este completo
		// Notificamos a la logica de que hemos procesado el frame
		if (c.tipo == RenderCommandType::PRESENT_FRAME) {
#ifdef PLATFORM_PS4
				sceRazorCpuPushMarker("PRESENT", SCE_RAZOR_CPU_COLOR_MAGENTA, 3);
#endif
			Renderer::Present();
#ifdef PLATFORM_PS4
				sceRazorCpuPopMarker();
#endif
			if (--_frames < maxQueue)
				SignalLock();
		}
	}
}

void RenderThread::WaitLock()
{
	_cv.wait(_lock);
}

void RenderThread::SignalLock()
{
	_cv.notify_one();
}