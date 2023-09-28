#ifndef PAINTER_HPP
#define PAINTER_HPP

#include <QThread>
#include "Window.hpp"
#include "Context.hpp"
#include <QMutex>

class Painter : public QThread
{
	Q_OBJECT
public:
	Painter(Window* windowTarget, QThread* render_thread = QThread::currentThread());
	virtual ~Painter();

public slots:
	void WindowResized(uint32_t width, uint32_t height);
	void WindowClosed();

protected:
	void run() override;

private:
	bool m_Running;
	std::shared_ptr<Context> m_Context;

	std::mutex m_Mutex;
};

#endif
