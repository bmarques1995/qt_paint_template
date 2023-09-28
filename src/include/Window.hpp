#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <QWindow>
#include <cstdint>

class Window : public QWindow
{
	Q_OBJECT
public:
	Window(uint32_t width = 1280, uint32_t height = 720);
	virtual ~Window() = default;

	void ConnectSignals();
signals:
	void WindowResized(uint32_t width, uint32_t height);
	void WindowClosed();

private:
	bool event(QEvent* event) override;

	void exposeEvent(QExposeEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
	//QCloseEvent
};

#endif