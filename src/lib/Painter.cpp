#include "Painter.hpp"

#include <windows.h>

Painter::Painter(Window* windowTarget, QThread* render_thread) :
	m_Running(true)
{
	connect(windowTarget, &Window::WindowResized, this, &Painter::WindowResized, Qt::AutoConnection);
	connect(windowTarget, &Window::WindowClosed, this, &Painter::WindowClosed, Qt::AutoConnection);
	connect(this, &Painter::RecreateWindow, windowTarget, &Window::RecreateWindow, Qt::QueuedConnection);
	emit RecreateWindow();
	uint32_t _width = windowTarget->width();
	uint32_t _height = windowTarget->height();
	HWND windowHandle = reinterpret_cast<HWND>(windowTarget->winId());
	m_Context.reset(new Context(windowHandle, _width, _height));
	m_Context->SetClearColor(.0f, .5f, .25f, 1.0f);


	std::initializer_list<BufferElement> bufferElements = {
			{ShaderDataType::Float3, "POSITION", false},
			{ShaderDataType::Float4, "COLOR", false}
	};

	m_Shader.reset(new Shader(m_Context.get(), "./triangle", bufferElements));
	m_VertexBuffer.reset(new VertexBuffer((const void*)vBuffer, sizeof(vBuffer), m_Shader->GetStride(), m_Context.get()));
	m_IndexBuffer.reset(new IndexBuffer((const void*)iBuffer, sizeof(iBuffer) / sizeof(uint32_t), m_Context.get()));
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
		m_Shader->Stage();
		m_VertexBuffer->Stage();
		m_IndexBuffer->Stage();
		m_Context->Draw(m_IndexBuffer->GetCount());
		m_Context->Present();
	}
}
