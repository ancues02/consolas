#include "RenderThread.h"
#include <cstdint>
#include "../Renderer/Renderer.h"


std::thread RenderThread::_renderThread;
std::atomic<bool> RenderThread::_exit = false;
std::atomic<unsigned int> RenderThread::_frames = 0;
moodycamel::ReaderWriterQueue<RenderCommand*> RenderThread::_q;


void RenderThread::Start()
{
	//crea el hilo y llama a run desde el hilo
	//_renderThread = std::thread([this]() { run(); });
	_renderThread = std::thread(run);

}

void RenderThread::Stop()
{
	_exit = true;
	RenderCommand* com;
	_q.enqueue(com);
	//hacer el join del hilo
	_renderThread.join();
}

void RenderThread::addCommand(RenderCommand* command)
{
	_q.enqueue(command);
	if (command->tipo == RenderCommandType::PRESENT_FRAME) {
		++_frames;
	}
}

void RenderThread::run()
{
	while (!_exit) {
		RenderCommand* c = *_q.peek();
		_q.pop();
		while (c != nullptr && c->tipo != RenderCommandType::PRESENT_FRAME) {
			switch (c->tipo)
			{
			case(RenderCommandType::CLEAR_RECT):
				Renderer::DrawRect(c->clearRectInfo.x1, c->clearRectInfo.y1, c->clearRectInfo.x2, c->clearRectInfo.y2, c->clearRectInfo.color);
				break;
			case(RenderCommandType::DRAW_TEXTURE):

				break;
			default:
				break;
			}
			c = *_q.peek();
			_q.pop();
		}
		if (c->tipo == RenderCommandType::PRESENT_FRAME) {
			Renderer::Present();
			--_frames;
		}

	}

}
