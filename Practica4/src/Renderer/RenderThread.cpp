#include "RenderThread.h"
#include <cstdint>
#include "../Renderer/Renderer.h"

void RenderThread::Start()
{
	//crea el hilo y llama a run desde el hilo
}

void RenderThread::Stop()
{
	_exit = true;
	RenderCommand* com;
	_q.enqueue(com);
	//hacer el join del hilo
}

void RenderThread::addCommand(RenderCommand* command)
{
	_q.enqueue(command);
	if (command->tipo == RenderCommandType::END_FRAME) {
		++_frames;
	}
}

void RenderThread::run()
{
	while (!_exit) {
		RenderCommand* c = *_q.peek();
		_q.pop();
		while (c != nullptr && c->tipo != RenderCommandType::END_FRAME) {
			switch (c->tipo)
			{
			case(RenderCommandType::CLEAR):

				break;
			case(RenderCommandType::DRAW_IMAGE):

				break;
			case(RenderCommandType::DRAW_LINE):

				break;
			default:
				break;
			}
			c = *_q.peek();
			_q.pop();
		}
		if (c->tipo == RenderCommandType::END_FRAME) {
			Renderer::Present();
			--_frames;
		}

	}

}
