#include "../concurrent_queue.h"

#include "RenderThread.h"
#include <cstdint>
#include "../Renderer/Renderer.h"
#include <chrono>

std::thread RenderThread::_renderThread;
std::atomic<bool> RenderThread::_exit = false;
std::atomic<unsigned int> RenderThread::_frames = 0;
ConcurrentQueue<RenderCommand> RenderThread::_q;
std::condition_variable RenderThread::_cv;
std::mutex RenderThread::_mutex;
const short RenderThread::maxEnculados = 2;

void RenderThread::Start()
{
	//crea el hilo y llama a run desde el hilo
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

void RenderThread::addCommand(RenderCommand command)
{
	_q.Enqueue(command);
	if (command.tipo == RenderCommandType::PRESENT_FRAME) {
		++_frames;
		_cv.notify_one();
	}
}

void RenderThread::run()
{
	while (!_exit) {
		RenderCommand c;
		_q.Dequeue(c);
		
		if (&c == nullptr) {
			std::unique_lock<std::mutex> lock(_mutex);
			RenderThread::_cv.wait(lock);
		}
		while (c.tipo != RenderCommandType::PRESENT_FRAME) {
			switch (c.tipo)
			{
			case(RenderCommandType::CLEAR_RECT):
				Renderer::DrawRect(c.clearRectInfo.x1, c.clearRectInfo.y1, c.clearRectInfo.x2, c.clearRectInfo.y2, c.clearRectInfo.color);
				break;
			case(RenderCommandType::DRAW_TEXTURE):
				Renderer::DrawImageColumn(*c.drawTextureLineInfo.image,
					c.drawTextureLineInfo.texX, c.drawTextureLineInfo.x1, c.drawTextureLineInfo.y1, c.drawTextureLineInfo.x2, c.drawTextureLineInfo.y2);
				break;
			default:
				break;
			}
			_q.Dequeue(c);
			
		}
		if (c.tipo == RenderCommandType::PRESENT_FRAME) {
			Renderer::Present();
			if (--_frames < maxEnculados)
				_cv.notify_one();
		}
	}
}
