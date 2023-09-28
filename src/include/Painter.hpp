#ifndef PAINTER_HPP
#define PAINTER_HPP

#include <QThread>
#include <QWindow>
#include "Context.hpp"

class Painter : public QThread
{
public:
	Painter(QWindow* windowTarget, QThread* render_thread = QThread::currentThread());
	virtual ~Painter();

	void ResizeSwapChain();

protected:
	void run() override;

private:
	bool m_Running;
	std::shared_ptr<Context> m_Context;
};

#endif
