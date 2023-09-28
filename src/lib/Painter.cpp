#include "Painter.hpp"
#include <windows.h>

Painter::Painter(Window* windowTarget, QThread* render_thread) :
	m_Running(true)
{
	connect(windowTarget, &Window::WindowResized, this, &Painter::WindowResized, Qt::AutoConnection);
	connect(windowTarget, &Window::WindowClosed, this, &Painter::WindowClosed, Qt::AutoConnection);
	uint32_t _width = windowTarget->width();
	uint32_t _height = windowTarget->height();
	HWND windowHandle = reinterpret_cast<HWND>(windowTarget->winId());
	m_Context.reset(new Context(windowHandle, _width, _height));
	m_Context->SetClearColor(.0f, .5f, .25f, 1.0f);
}

Painter::~Painter()
{
}

void Painter::WindowResized(uint32_t width, uint32_t height)
{
	if (m_Running)
	{
		m_Mutex.lock();
		m_Context->OnResize(width, height);
		m_Mutex.unlock();
	}
}

void Painter::WindowClosed()
{
	m_Mutex.lock();
	m_Running = false;
	m_Mutex.unlock();
}

void Painter::run()
{
	while (m_Running)
	{
		m_Context->ClearTarget();
		m_Context->Present();
	}
}
