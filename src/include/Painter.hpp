#ifndef PAINTER_HPP
#define PAINTER_HPP

#include <QThread>
#include "Window.hpp"
#include "Context.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"
#include <QMutex>

class Painter : public QThread
{
	Q_OBJECT
public:
	Painter(Window* windowTarget, QThread* render_thread = QThread::currentThread());
	virtual ~Painter();

signals:
	void RecreateWindow();

public slots:
	void WindowResized(uint32_t width, uint32_t height);
	void WindowClosed();

protected:
	void run() override;

private:
	bool m_Running;
	std::shared_ptr<Context> m_Context;
	std::shared_ptr<Shader> m_Shader;
	std::shared_ptr<VertexBuffer> m_VertexBuffer;
	std::shared_ptr<IndexBuffer> m_IndexBuffer;

	std::mutex m_Mutex;

	float vBuffer[21] =
	{
		.0f, .5f, .0f, 1.0f, .0f, .0f, 1.0f,
		.5f, -.5f, .0f, .0f, 1.0f, .0f, 1.0f,
		-.5f, -.5f, .0f, .0f, .0f, 1.0f, 1.0f
	};

	uint32_t iBuffer[3] =
	{
		0,1,2
	};
};

#endif
