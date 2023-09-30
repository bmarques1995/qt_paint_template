#include "Window.hpp"
#include <windows.h>

Window::Window(uint32_t width, uint32_t height) :
	QWindow()
{
	setBaseSize(QSize(width, height));
}

void Window::ConnectSignals()
{
}

bool Window::event(QEvent* event)
{
    switch (event->type())
    {
    default:
        return QWindow::event(event);
    }
}

void Window::RecreateWindow()
{
}

void Window::exposeEvent(QExposeEvent* event)
{
	Q_UNUSED(event);
}

void Window::resizeEvent(QResizeEvent* event)
{
	emit WindowResized(width(), height());
}

void Window::closeEvent(QCloseEvent* event)
{
	emit WindowClosed();
}
