#include "Painter.hpp"
#include <windows.h>

Painter::Painter(QWindow* windowTarget, QThread* render_thread) :
	m_Running(true)
{
	uint32_t _width = windowTarget->width();
	uint32_t _height = windowTarget->height();
	HWND windowHandle = reinterpret_cast<HWND>(windowTarget->winId());
	m_Context.reset(new Context(windowHandle, _width, _height));
	m_Context->SetClearColor(.0f, .5f, .25f, 1.0f);
}

Painter::~Painter()
{
}

void Painter::ResizeSwapChain()
{
}

void Painter::run()
{
	while (m_Running)
	{
		m_Context->ClearTarget();
		m_Context->Present();
	}
}
