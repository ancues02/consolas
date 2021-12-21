#include "../concurrent_queue.h"

#include "RenderThread.h"
#include <cstdint>
#include "../Renderer/Renderer.h"
//#include "../Utils.h"
#include <chrono>

#ifdef PLATFORM_PS4
#include <razorcpu.h>
#include <razorcpu_debug.h>
#endif

std::thread RenderThread::_renderThread;
std::atomic<bool> RenderThread::_exit;
std::atomic<unsigned int> RenderThread::_frames;
ConcurrentQueue<RenderCommand> RenderThread::_q;
std::condition_variable RenderThread::_cv;
std::mutex RenderThread::_mutex;
std::unique_lock<std::mutex> RenderThread::_lock(_mutex);
const short RenderThread::maxQueue = 2;

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
	RenderCommand com;
	_q.Enqueue(com);
	//hacer el join del hilo
	_renderThread.join();
}

void RenderThread::IncreaseFrames() {
	_frames++;
}

void RenderThread::addCommand(const RenderCommand& command)
{
	_q.Enqueue(command);
}

void RenderThread::run()
{
#ifdef PLATFORM_PS4
	scePthreadSetaffinity(scePthreadSelf(), 1 << 3);
#endif
	while (!_exit) {
		if (_q.size() <= 0) {
			_cv.wait(_lock);
		}
		RenderCommand c;		
		_q.Dequeue(c);

		while (c.tipo != RenderCommandType::PRESENT_FRAME) {
			switch (c.tipo)
			{
			case(RenderCommandType::CLEAR_RECT):
#ifdef PLATFORM_PS4
				sceRazorCpuPushMarker("SPLIT", SCE_RAZOR_CPU_COLOR_GREEN, 1);
#endif
				Renderer::SplitClear(c.clearRectInfo.color1, c.clearRectInfo.color2);
#ifdef PLATFORM_PS4
				sceRazorCpuPopMarker();
#endif
				break;
			case(RenderCommandType::DRAW_TEXTURE):
/*#ifdef PLATFORM_PS4
				sceRazorCpuPushMarker("COLUMN", SCE_RAZOR_CPU_COLOR_BLUE, 2);
#endif*/
				Renderer::DrawImageColumn(*c.drawTextureLineInfo.image,
					c.drawTextureLineInfo.texX, c.drawTextureLineInfo.x1, c.drawTextureLineInfo.y1, c.drawTextureLineInfo.x2, c.drawTextureLineInfo.y2);
/*#ifdef PLATFORM_PS4
				sceRazorCpuPopMarker();
#endif*/
				break;
			case(RenderCommandType::RAY_LINES):
#ifdef PLATFORM_PS4
				sceRazorCpuPushMarker("COLUMN", SCE_RAZOR_CPU_COLOR_BLUE, 2);
#endif
				for (int i = 0; i < Renderer::GetWidth(); i++) {
					Renderer::DrawImageColumn(*c.rayLinesInfo.rl[i].im,
						c.rayLinesInfo.rl[i].texX, c.rayLinesInfo.rl[i].x1, c.rayLinesInfo.rl[i].y1, c.rayLinesInfo.rl[i].x2, c.rayLinesInfo.rl[i].y2);
				}
#ifdef PLATFORM_PS4
				sceRazorCpuPopMarker();
#endif

				break;
			default:
				break;
			}
			_q.Dequeue(c);
			
		}
		if (c.tipo == RenderCommandType::PRESENT_FRAME) {
			Renderer::Present();
			//--_frames;
			if (--_frames < maxQueue)
				_cv.notify_one();
		}
	}
}
