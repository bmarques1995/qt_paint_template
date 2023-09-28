#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <QWindow>
#include <cstdint>

class Window : public QWindow
{
public:
	Window(uint32_t width = 1280, uint32_t height = 720);
	virtual ~Window() = default;

private:

};

#endif