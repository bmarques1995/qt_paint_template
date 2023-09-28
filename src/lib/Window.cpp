#include "Window.hpp"

Window::Window(uint32_t width, uint32_t height) :
	QWindow()
{
	setWidth(width);
	setHeight(height);
}
